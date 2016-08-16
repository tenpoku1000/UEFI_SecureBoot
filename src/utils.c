
// Copyright 2015 Shin'ichi Ichikawa. Released under the MIT license.

#include "utils.h"
#include "efi_status.h"

static bool exit_boot_services = false;

void set_exit_boot_services(void)
{
    exit_boot_services = true;
}

static bool is_exit_boot_services(void)
{
    return exit_boot_services;
}

static bool reboot_msg = true;

void set_reboot_no_msg(void)
{
    reboot_msg = false;
}

static bool is_reboot_msg(void)
{
    return reboot_msg;
}

void read_key(void)
{
    if (ST->ConIn){

        EFI_STATUS local_status = EFI_SUCCESS;

        do{
            EFI_INPUT_KEY key;

            local_status = ST->ConIn->ReadKeyStroke(ST->ConIn, &key);

        } while (EFI_SUCCESS != local_status);
    }
}

void reset_system(EFI_STATUS status)
{
    if (false == is_exit_boot_services()) {

        if (is_reboot_msg()) {

            Print(L"\n");
            Print(L"When you press any key, the system will reboot.\n");
        }

        read_key();
    }

    RT->ResetSystem(EfiResetCold, status, 0, NULL);
}

void error_print(CHAR16* msg, EFI_STATUS* status)
{
    if (false == is_exit_boot_services()){

        Print(msg);

        if (status){

            Print(L"EFI_STATUS = %d, %s\n", *status, print_status_msg(*status));
        }
    }

    reset_system(EFI_SUCCESS);
}



// Copyright 2015 Shin'ichi Ichikawa. Released under the MIT license.

#include "setup_screen.h"

#define EFI_OS_INDICATIONS_BOOT_TO_FW_UI 0x0000000000000001

static EFI_STATUS get_variable_u64(CHAR16* variable_name, UINT32* attr, UINT64* data)
{
    UINTN data_size = sizeof(UINT64);

    EFI_STATUS status = RT->GetVariable(variable_name, &EfiGlobalVariable, attr, &data_size, data);

    switch (status){
    case EFI_NOT_FOUND:
//        break;
    case EFI_SUCCESS:
        return EFI_SUCCESS;
    default:
        break;
    }

    return status;
}

static EFI_STATUS set_variable_u64(CHAR16* variable_name, UINT32 attr, UINT64* data)
{
    UINTN data_size = sizeof(UINT64);

    EFI_STATUS status = RT->SetVariable(variable_name, &EfiGlobalVariable, attr, data_size, data);

    switch (status){
    case EFI_SUCCESS:
        return EFI_SUCCESS;
    default:
        break;
    }

    return status;
}

static bool is_boot_to_fw_ui(void)
{
    UINT32 attr = 0;
    UINT64 os_indications_supported = 0;

    EFI_STATUS status = get_variable_u64(L"OsIndicationsSupported", &attr, &os_indications_supported);

    if (EFI_ERROR(status)){

        error_print(L"get_variable_u64(OsIndicationsSupported) failed.\n", &status);
    }

    if (EFI_OS_INDICATIONS_BOOT_TO_FW_UI & os_indications_supported){

        return true;
    }

    return false;
}

static void set_boot_to_fw_ui(void)
{
    UINT32 attr = EFI_VARIABLE_NON_VOLATILE | EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_RUNTIME_ACCESS;

    UINT64 os_indications = 0;

    os_indications |= EFI_OS_INDICATIONS_BOOT_TO_FW_UI;

    EFI_STATUS status = set_variable_u64(L"OsIndications", attr, &os_indications);

    if (EFI_ERROR(status)){

        error_print(L"set_variable_u64(OsIndications) failed.\n", &status);
    }
}

void transition_setup_screen(bool secure_boot_enable)
{
    if (is_boot_to_fw_ui()){

        Print(L"When you press any key, The system will transition to the UEFI setup screen.\n");

        if (secure_boot_enable){

            Print(L"After please activate the UEFI secure boot.");

        }else{

            Print(L"After please clear the UEFI key.");
        }

        set_boot_to_fw_ui();

        set_reboot_no_msg();

    }else{

        Print(L"Manually please switch to the UEFI setup screen and please clear the UEFI key.\n");
    }

    reset_system(EFI_SUCCESS);
}


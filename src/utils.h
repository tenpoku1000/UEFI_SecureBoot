
// Copyright 2015 Shin'ichi Ichikawa. Released under the MIT license.

#if ! defined(UTILS_H_)
#define UTILS_H_

#include <efi.h>
#include <efilib.h>

#define true 1
#define false 0

typedef UINT8 bool;

void set_exit_boot_services(void);
void set_reboot_no_msg(void);
void read_key(void);
void reset_system(EFI_STATUS status);
void error_print(CHAR16* msg, EFI_STATUS* status);

#endif


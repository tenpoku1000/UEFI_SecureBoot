
// Copyright 2015 Shin'ichi Ichikawa. Released under the MIT license.

#if ! defined(TP_UDKENV_H_)
#define TP_UDKENV_H_

void tp_set_uefi_pointers(EFI_HANDLE image_handle, EFI_SYSTEM_TABLE* system_table);

typedef enum ENROLL_UEFI_KEY_ {
    ENROLL_UEFI_KEY_PK,
    ENROLL_UEFI_KEY_KEK,
    ENROLL_UEFI_KEY_DB,
}ENROLL_UEFI_KEY;

EFI_STATUS tp_begin_enroll_keys(void);

EFI_STATUS tp_enroll_uefi_key(
    ENROLL_UEFI_KEY file_type, EFI_FILE_HANDLE efi_file, CHAR16* file_name
);

void tp_end_enroll_keys(void);

void tp_reset_cold(void);

EFI_STATUS tp_blt_lib_configure(
    VOID*                                 FrameBuffer,
    EFI_GRAPHICS_OUTPUT_MODE_INFORMATION* FrameBufferInfo
);

EFI_STATUS tp_blt_lib_gop_blt(
    EFI_GRAPHICS_OUTPUT_BLT_PIXEL*        BltBuffer,
    EFI_GRAPHICS_OUTPUT_BLT_OPERATION     BltOperation,
    UINTN                                 SourceX,
    UINTN                                 SourceY,
    UINTN                                 DestinationX,
    UINTN                                 DestinationY,
    UINTN                                 Width,
    UINTN                                 Height,
    UINTN                                 Delta
);

#endif


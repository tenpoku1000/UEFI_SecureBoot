
// Copyright 2015 Shin'ichi Ichikawa. Released under the MIT license.

#include <UDK2014.h>
#include <SecureBootConfigDxe/SecureBootConfigImpl.h>
#include <Library/ResetSystemLib.h>
#include <Library/BltLib.h>
#include "tp_UDKenv.h"
#include "tp_uefi_memory.h"

void tp_set_uefi_pointers(EFI_HANDLE image_handle, EFI_SYSTEM_TABLE* system_table)
{
    (void)UefiBootServicesTableLibConstructor(image_handle, system_table);
    (void)UefiRuntimeServicesTableLibConstructor(image_handle, system_table);
}

static SECUREBOOT_CONFIG_PRIVATE_DATA* private_data = NULL;

EFI_STATUS tp_begin_enroll_keys(void)
{
    private_data = AllocateCopyPool(
        sizeof(SECUREBOOT_CONFIG_PRIVATE_DATA), &mSecureBootConfigPrivateDateTemplate
    );

    if (NULL == private_data){

        tp_print(L"AllocateCopyPool() failed.\n");

        return EFI_OUT_OF_RESOURCES;
    }

    SECUREBOOT_FILE_CONTEXT* file_context = AllocateZeroPool(sizeof(SECUREBOOT_FILE_CONTEXT));

    if (NULL == file_context){

        tp_print(L"AllocateZeroPool() failed.\n");

        FreePool(private_data);
        private_data = NULL;

        return EFI_OUT_OF_RESOURCES;
    }

    private_data->FileContext = file_context;

    private_data->SignatureGUID = AllocateZeroPool(sizeof(EFI_GUID));

    if (NULL == private_data->SignatureGUID){

        tp_print(L"AllocateZeroPool() failed.\n");

        FreePool(file_context);
        file_context = NULL;

        FreePool(private_data);
        private_data = NULL;

        return EFI_OUT_OF_RESOURCES;
    }

    return EFI_SUCCESS;
}

EFI_STATUS tp_enroll_uefi_key(ENROLL_UEFI_KEY file_type, EFI_FILE_HANDLE efi_file, CHAR16* file_name)
{
    if (NULL == private_data){

        tp_print(L"EFI_OUT_OF_RESOURCES\n");

        return EFI_OUT_OF_RESOURCES;

    }else{

        if (NULL == private_data->FileContext){

            tp_print(L"EFI_OUT_OF_RESOURCES\n");

            return EFI_OUT_OF_RESOURCES;
        }
    }

    private_data->FileContext->FHandle = efi_file;
    private_data->FileContext->FileName = file_name;

    EFI_STATUS status = EFI_SUCCESS;

    switch (file_type){
    case ENROLL_UEFI_KEY_PK:

        status = EnrollPlatformKey(private_data);

        break;
    case ENROLL_UEFI_KEY_KEK:

        status = EnrollKeyExchangeKey(private_data);

        break;
    case ENROLL_UEFI_KEY_DB:

        status = EnrollSignatureDatabase(private_data, EFI_IMAGE_SECURITY_DATABASE);

        break;
    default:

        tp_print(L"EFI_INVALID_PARAMETER\n");

        return EFI_INVALID_PARAMETER;
    }

    return status;
}

void tp_end_enroll_keys(void)
{
    if (private_data){

        if (private_data->FileContext){

            FreePool(private_data->FileContext);
            private_data->FileContext = NULL;
        }

        if (private_data->SignatureGUID){

            FreePool(private_data->SignatureGUID);
            private_data->SignatureGUID = NULL;
        }

        FreePool(private_data);
        private_data = NULL;
    }
}

void tp_reset_cold(void)
{
    ResetCold();
}

EFI_STATUS tp_blt_lib_configure(
    VOID*                                 FrameBuffer,
    EFI_GRAPHICS_OUTPUT_MODE_INFORMATION* FrameBufferInfo)
{
    return BltLibConfigure(
        FrameBuffer, FrameBufferInfo
    );
}

EFI_STATUS tp_blt_lib_gop_blt(
    EFI_GRAPHICS_OUTPUT_BLT_PIXEL*        BltBuffer,
    EFI_GRAPHICS_OUTPUT_BLT_OPERATION     BltOperation,
    UINTN                                 SourceX,
    UINTN                                 SourceY,
    UINTN                                 DestinationX,
    UINTN                                 DestinationY,
    UINTN                                 Width,
    UINTN                                 Height,
    UINTN                                 Delta)
{
    return BltLibGopBlt(
        BltBuffer,
        BltOperation,
        SourceX,
        SourceY,
        DestinationX,
        DestinationY,
        Width,
        Height,
        Delta
    );
}


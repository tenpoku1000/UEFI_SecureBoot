
// Copyright 2015 Shin'ichi Ichikawa. Released under the MIT license.

#if ! defined(TP_UEFI_MEMORY_H_)
#define TP_UEFI_MEMORY_H_

static void tp_print(CHAR16* msg)
{
    gST->ConOut->OutputString(gST->ConOut, msg);
}

static void* AllocatePool(UINTN size)
{
    void* p = NULL;

    EFI_STATUS status = gBS->AllocatePool(EfiBootServicesData, size, &p);

    if (EFI_ERROR(status)){

        p = NULL;
    }

    return p;
}

static void* AllocateZeroPool(UINTN size)
{
    void* p = AllocatePool(size);

    if (p){

        gBS->SetMem(p, size, 0);
    }

    return p;
}

static void* AllocateCopyPool(UINTN size, void* p)
{
    void* q = AllocateZeroPool(size);

    if (q){

        gBS->CopyMem(q, p, size);
    }

    return p;
}

static void FreePool(void* p)
{
    (void)gBS->FreePool(p);
}

#endif


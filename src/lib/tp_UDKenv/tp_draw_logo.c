
// Copyright 2016 Shin'ichi Ichikawa. Released under the MIT license.

#include <UDK.h>
#include <Base.h>
#include <Guid/Acpi.h>
#include <IndustryStandard/Acpi.h>
#include <IndustryStandard/Bmp.h>
#include <Protocol/GraphicsOutput.h>
#include "tp_draw_logo.h"
#include "tp_uefi_memory.h"

static EFI_ACPI_5_1_ROOT_SYSTEM_DESCRIPTION_POINTER* acpi_rsdp = NULL;
// Boot Graphics Resource Table(BGRT)
static EFI_ACPI_5_1_BOOT_GRAPHICS_RESOURCE_TABLE* acpi_bgrt = NULL;

static void tp_get_bgrt(UINT8* table);
static void tp_draw_logo2(EFI_GRAPHICS_OUTPUT_PROTOCOL* gop);
static EFI_GRAPHICS_OUTPUT_BLT_PIXEL* conv_bitmap(UINTN* bitmap_width, UINTN* bitmap_height);

void tp_draw_logo(EFI_GRAPHICS_OUTPUT_PROTOCOL* gop)
{
    if (NULL == gop){

        return;
    }

    acpi_bgrt = NULL;

    for (UINTN i = 0; gST->NumberOfTableEntries > i; ++i){

        EFI_GUID* guid = &(gST->ConfigurationTable[i].VendorGuid);
        UINT8* table = (UINT8*)(gST->ConfigurationTable[i].VendorTable);

        if (0 == CompareMem(guid, &gEfiAcpiTableGuid, sizeof(EFI_GUID))){

            tp_get_bgrt(table);

            if (NULL == acpi_bgrt){

                return;
            }

            tp_draw_logo2(gop);

            break;
        }
    }
}

static void tp_get_bgrt(UINT8* table)
{
    if (NULL == table){

        return;
    }

    acpi_rsdp = (EFI_ACPI_5_1_ROOT_SYSTEM_DESCRIPTION_POINTER*)table;

    EFI_ACPI_DESCRIPTION_HEADER* acpi_header = (EFI_ACPI_DESCRIPTION_HEADER*)(acpi_rsdp->XsdtAddress);

    if (acpi_header){

        UINTN size = (acpi_header->Length - sizeof(*acpi_header)) / sizeof(void*);

        ++acpi_header;

        EFI_ACPI_DESCRIPTION_HEADER** header = (EFI_ACPI_DESCRIPTION_HEADER**)acpi_header;

        for (UINTN j = 0; size > j; ++j){

            if (EFI_ACPI_5_1_BOOT_GRAPHICS_RESOURCE_TABLE_SIGNATURE == header[j]->Signature){

                acpi_bgrt = (EFI_ACPI_5_1_BOOT_GRAPHICS_RESOURCE_TABLE*)header[j];

                if (EFI_ACPI_5_1_BOOT_GRAPHICS_RESOURCE_TABLE_REVISION != acpi_bgrt->Header.Revision){

                    acpi_bgrt = NULL;

                    break;
                }

                if (EFI_ACPI_5_1_BGRT_VERSION != acpi_bgrt->Version){

                    acpi_bgrt = NULL;

                    break;
                }

                if (EFI_ACPI_5_1_BGRT_IMAGE_TYPE_BMP != acpi_bgrt->ImageType){

                    acpi_bgrt = NULL;

                    break;
                }

                break;
            }
        }
    }
}

void tp_draw_logo2(EFI_GRAPHICS_OUTPUT_PROTOCOL* gop)
{
    if ((NULL == gop) || (NULL == acpi_bgrt)){

        return;
    }

    UINTN bitmap_width = 0;
    UINTN bitmap_height = 0;

    EFI_GRAPHICS_OUTPUT_BLT_PIXEL* p = conv_bitmap(&bitmap_width, &bitmap_height);

    if (NULL == p){

        return;
    }

    EFI_STATUS status = gop->Blt(
        gop, p,
        EfiBltBufferToVideo,
        0, 0,
        acpi_bgrt->ImageOffsetX, acpi_bgrt->ImageOffsetY,
        bitmap_width, bitmap_height,
        bitmap_width * sizeof(EFI_GRAPHICS_OUTPUT_BLT_PIXEL)
    );

    FreePool(p);
    p = NULL;

    if (EFI_ERROR(status)){

        return;
    }
}

static EFI_GRAPHICS_OUTPUT_BLT_PIXEL* conv_bitmap(UINTN* bitmap_width, UINTN* bitmap_height)
{
    if ((NULL == acpi_bgrt) || (NULL == bitmap_width) || (NULL == bitmap_height)){

        return NULL;
    }

    BMP_IMAGE_HEADER* bmp = (BMP_IMAGE_HEADER*)acpi_bgrt->ImageAddress;

    if (NULL == bmp){

        return NULL;
    }

    if (('B' != bmp->CharB) || ('M' != bmp->CharM)){

        return NULL;
    }

    if (bmp->HeaderSize != (sizeof(BMP_IMAGE_HEADER) - OFFSET_OF(BMP_IMAGE_HEADER, HeaderSize))){

        return NULL;
    }

    if (bmp->Size < bmp->ImageOffset){

        return NULL;
    }

    if (1 != bmp->Planes){

        return NULL;
    }

    if (24 != bmp->BitPerPixel){

        return NULL;
    }

    if (0 != bmp->CompressionType){

        return NULL;
    }

    UINT8* image_header = (UINT8*)acpi_bgrt->ImageAddress;
    UINT8* image = image_header + bmp->ImageOffset;

    UINTN width = bmp->PixelWidth;
    UINTN height = bmp->PixelHeight;

    *bitmap_width = width;
    *bitmap_height = height;

    UINTN num = width * height;

    EFI_GRAPHICS_OUTPUT_BLT_PIXEL* p = AllocateZeroPool(num * sizeof(EFI_GRAPHICS_OUTPUT_BLT_PIXEL));

    if (NULL == p){

        return NULL;
    }

    for (UINTN i = 0; i < height; ++i){

        EFI_GRAPHICS_OUTPUT_BLT_PIXEL* q = &p[(height - i - 1) * width];

        for (UINTN j = 0; j < width; ++j){

            q->Green = *image;
            ++image;

            q->Red = *image;
            ++image;

            q->Blue = *image;
            ++image;

//          q->Reserved;

            ++q;
        }

        UINTN image_index = (UINTN)(image - image_header);

        if (image_index % 4){

            image = image + (4 - (image_index % 4));
        }
    }

    return p;
}


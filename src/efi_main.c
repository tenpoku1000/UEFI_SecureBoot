
// Copyright 2015 Shin'ichi Ichikawa. Released under the MIT license.

#include "secure_boot_mode.h"
#include "setup_screen.h"
#include "tp_UDKenv.h"

#define SCREEN_WIDTH 1024

static EFI_HANDLE IH = NULL;
static EFI_GRAPHICS_OUTPUT_PROTOCOL* gop = NULL;
static EFI_LOADED_IMAGE* loaded_image = NULL;

typedef struct mem_map_ {
    UINTN memory_map_size;
    EFI_MEMORY_DESCRIPTOR* memory_map;
    UINTN map_key;
    UINTN descriptor_size;
    UINT32 descriptor_version;
}mem_map;

static mem_map* MM = NULL;

typedef enum UEFI_KEYS_{
    UEFI_KEYS_EXISTS,
    UEFI_KEYS_OPEN,
}UEFI_KEYS_METHOD;

static void init(EFI_HANDLE image_handle, EFI_SYSTEM_TABLE* system_table);
static void boot_exec(void);
static bool file_name_match(CHAR16* path, CHAR16* file_name);
static void boot_self_signed(void);
static bool uefi_keys(
    UEFI_KEYS_METHOD method,
    EFI_FILE_HANDLE* pk_file, EFI_FILE_HANDLE* kek_file, EFI_FILE_HANDLE* db_file
);
static void regist_uefi_keys(void);
static void boot_common(void);
static void exit_boot_services(void);
static void get_memory_map(void);

EFI_STATUS efi_main(EFI_HANDLE image_handle, EFI_SYSTEM_TABLE* system_table)
{
    init(image_handle, system_table);

    boot_exec();

    return EFI_SUCCESS;
}

static void init(EFI_HANDLE image_handle, EFI_SYSTEM_TABLE* system_table)
{
    InitializeLib(image_handle, system_table);

    IH = image_handle;

    tp_set_uefi_pointers(image_handle, system_table);

    EFI_STATUS status = EFI_SUCCESS;

    if ((NULL == ST->ConIn) || (EFI_SUCCESS != (status = ST->ConIn->Reset(ST->ConIn, 0)))){

        error_print(L"Input device unavailable.\n", ST->ConIn ? &status : NULL);
    }

    status = BS->OpenProtocol(
        image_handle, &LoadedImageProtocol, &loaded_image,
        image_handle, NULL, EFI_OPEN_PROTOCOL_GET_PROTOCOL
    );

    if (EFI_ERROR(status)){

        error_print(L"OpenProtocol() LoadedImageProtocol failed.\n", &status);
    }

    UINTN number_gop_handles = 0;
    EFI_HANDLE* gop_handles = NULL;

    status = BS->LocateHandleBuffer(
        ByProtocol,
        &GraphicsOutputProtocol,
        NULL,
        &number_gop_handles,
        &gop_handles
    );

    if (EFI_ERROR(status)){

        error_print(L"LocateHandleBuffer() GraphicsOutputProtocol failed.\n", &status);
    }

    status = BS->OpenProtocol(
        gop_handles[0], &GraphicsOutputProtocol, &gop,
        image_handle, NULL, EFI_OPEN_PROTOCOL_GET_PROTOCOL
    );

    if (EFI_ERROR(status)){

        error_print(L"OpenProtocol() GraphicsOutputProtocol failed.\n", &status);
    }

    FreePool(gop_handles);
    gop_handles = NULL;

    int mode_num = 0;
    int set_mode_num = 0;
    bool valid_query_mode = false;
    EFI_GRAPHICS_OUTPUT_MODE_INFORMATION* mode_info;
    UINTN size = 0;
    UINT32 HorizontalResolution = 0;

    for (; (status = gop->QueryMode(gop, mode_num, &size, &mode_info)) == EFI_SUCCESS; ++mode_num){

        if (PixelBlueGreenRedReserved8BitPerColor != mode_info->PixelFormat) {

            continue;
        }

        if (HorizontalResolution < mode_info->HorizontalResolution){

            HorizontalResolution = mode_info->HorizontalResolution;

            set_mode_num = mode_num;

            valid_query_mode = true;

            if (HorizontalResolution == SCREEN_WIDTH) {

                break;
            }
        }
    }

    if (false == valid_query_mode){

        error_print(L"QueryMode() GOP failed.\n", NULL);
    }

    status = gop->SetMode(gop, set_mode_num);

    if (EFI_ERROR(status)){

        error_print(L"SetMode() GOP failed.\n", &status);
    }
}

static void boot_exec()
{
    CHAR16* path = DevicePathToStr(loaded_image->FilePath);

    if (path){

        if (file_name_match(path, L"GRUBX64.EFI")){

            boot_common();

        }else if (file_name_match(path, L"BOOTX64.EFI")){

            boot_self_signed();

        }else{

            Print(L"Bad file name(%s).\n", path);
        }

        FreePool(path);
        path = NULL;
    }else{

        error_print(L"DevicePathToStr() failed.\n", NULL);
    }
}

static bool file_name_match(CHAR16* path, CHAR16* file_name)
{
    UINTN file_name_length = StrLen(file_name);

    for (UINTN i = 0; L'\0' != path[i]; ++i){

        if (L'\\' == path[i]){

            ++i;

            if (L'\0' == path[i]){

                break;
            }

            CHAR16* str = path + i;

            UINTN length = StrLen(str);

            if (length > file_name_length){

                length = file_name_length;
            }

            if (0 == StrnCmp(str, file_name, length)){

                return true;
            }
        }
    }

    return false;
}

static void boot_self_signed(void)
{
    bool is_exists_keys = uefi_keys(UEFI_KEYS_EXISTS, NULL, NULL, NULL);

    if (is_exists_keys){

        bool is_unknown_mode = false;

        if (is_secure_boot_setup_mode(&is_unknown_mode)){

            regist_uefi_keys();

        }else{

            if (is_unknown_mode){

                boot_common();

            }else{

                transition_setup_screen(false);
            }
        }
    }else{

        boot_common();
    }
}

static bool uefi_keys(
    UEFI_KEYS_METHOD method,
    EFI_FILE_HANDLE* pk_file, EFI_FILE_HANDLE* kek_file, EFI_FILE_HANDLE* db_file)
{
    EFI_FILE_IO_INTERFACE* efi_simple_file_system = NULL;
    EFI_FILE* efi_file_root = NULL;
    EFI_FILE* efi_file = NULL;

    EFI_STATUS status = BS->OpenProtocol(
        loaded_image->DeviceHandle,
        &FileSystemProtocol,
        &efi_simple_file_system,
        IH,
        NULL,
        EFI_OPEN_PROTOCOL_GET_PROTOCOL
    );

    if (EFI_ERROR(status)){

        error_print(L"OpenProtocol() FileSystemProtocol failed.\n", &status);
    }

    status = efi_simple_file_system->OpenVolume(
        efi_simple_file_system, &efi_file_root
    );

    if (EFI_ERROR(status)){

        error_print(L"OpenVolume() failed.\n", &status);
    }

    CHAR16* path[] = {
        L"\\EFI\\BOOT\\PKSELF.CER",
        L"\\EFI\\BOOT\\KEKSELF.CER",
        L"\\EFI\\BOOT\\DBSELF.CER",
        NULL
    };

    enum PK_KEK_INDEX_{
        PK_INDEX = 0,
        KEK_INDEX,
        DB_INDEX,
    };

    for (int i = 0; NULL != path[i]; ++i){

        status = efi_file_root->Open(
            efi_file_root, &efi_file, path[i],
            EFI_FILE_MODE_READ | EFI_FILE_MODE_WRITE, 0
        );

        if ((UEFI_KEYS_EXISTS == method) && (EFI_NOT_FOUND == status)){

            return false;
        }

        if (EFI_ERROR(status)){

            error_print(L"Open() failed.\n", &status);
        }

        if (UEFI_KEYS_OPEN == method){

            switch (i) {
            case PK_INDEX:
                *pk_file = efi_file;
                break;
            case KEK_INDEX:
                *kek_file = efi_file;
                break;
            case DB_INDEX:
                *db_file = efi_file;
                break;
            default:
                error_print(L"Bad index value.\n", NULL);
            }
        }
    }

    return true;
}

static void regist_uefi_keys(void)
{
    EFI_STATUS status = tp_begin_enroll_keys();

    if (EFI_ERROR(status)){

        error_print(L"tp_begin_enroll_keys() failed.\n", &status);
    }

    EFI_FILE_HANDLE pk_file = NULL;
    EFI_FILE_HANDLE kek_file = NULL;
    EFI_FILE_HANDLE db_file = NULL;

    if (false == uefi_keys(UEFI_KEYS_OPEN, &pk_file, &kek_file, &db_file)){

        error_print(L"uefi_keys(UEFI_KEYS_OPEN) failed.\n", NULL);
    }

    CHAR16* db_file_name = L"dbself.cer";

    status = tp_enroll_uefi_key(
        ENROLL_UEFI_KEY_DB, db_file, db_file_name
    );

    if (EFI_ERROR(status)){

        (void)pk_file->Close(pk_file);
        (void)kek_file->Close(kek_file);
        (void)db_file->Close(db_file);

        error_print(L"tp_enroll_uefi_key(ENROLL_UEFI_KEY_DB) failed.\n", &status);
    }

    CHAR16* kek_file_name = L"kekself.cer";

    status = tp_enroll_uefi_key(
        ENROLL_UEFI_KEY_KEK, kek_file, kek_file_name
    );

    if (EFI_ERROR(status)){

        (void)pk_file->Close(pk_file);
        (void)kek_file->Close(kek_file);
        (void)db_file->Close(db_file);

        error_print(L"tp_enroll_uefi_key(ENROLL_UEFI_KEY_KEK) failed.\n", &status);
    }

    CHAR16* pk_file_name = L"pkself.cer";

    status = tp_enroll_uefi_key(
        ENROLL_UEFI_KEY_PK, pk_file, pk_file_name
        );

    if (EFI_ERROR(status)){

        (void)pk_file->Close(pk_file);
        (void)kek_file->Close(kek_file);
        (void)db_file->Close(db_file);

        error_print(L"tp_enroll_uefi_key(ENROLL_UEFI_KEY_PK) failed.\n", &status);
    }

    tp_end_enroll_keys();

    status = pk_file->Delete(db_file);

    if (EFI_ERROR(status)){

        error_print(L"Delete(DB) failed.\n", &status);
    }

    status = pk_file->Delete(pk_file);

    if (EFI_ERROR(status)){

        error_print(L"Delete(PK) failed.\n", &status);
    }

    pk_file = NULL;

    status = kek_file->Delete(kek_file);

    if (EFI_ERROR(status)){

        error_print(L"Delete(KEK) failed.\n", &status);
    }

    kek_file = NULL;

    Print(L"Successful registration of PK, KEK and DB.\n");

    transition_setup_screen(true);
}

static void boot_common(void)
{
    Print(L"After pressing any key, the system is reset in the case of a successful execution of ExitBootServices().\n");

    read_key();

    exit_boot_services();

    tp_reset_cold();
}

static void exit_boot_services(void)
{
    EFI_STATUS status = BS->AllocatePool(EfiLoaderData, sizeof(*MM), &MM);

    if (EFI_ERROR(status)){

        error_print(L"AllocatePool() failed.\n", &status);
    }

    BS->SetMem(MM, sizeof(*MM), 0);

    bool retry = false;

retry:
    ;

    get_memory_map();

    status = BS->ExitBootServices(IH, MM->map_key);

    if (EFI_ERROR(status)){

        if (retry){

            error_print(L"ExitBootServices() faild.\n", &status);
        }

        retry = true;

        goto retry;
    }

    BS = NULL;

    set_exit_boot_services();
}

static void get_memory_map(void)
{
    MM->memory_map_size = 0;

    for (;;){

        if (MM->memory_map){

            FreePool(MM->memory_map);
            MM->memory_map = NULL;
        }

        EFI_STATUS status = BS->AllocatePool(EfiLoaderData, MM->memory_map_size, &MM->memory_map);

        if (EFI_ERROR(status)){

            error_print(L"AllocatePool() failed.\n", &status);
        }

        status = BS->GetMemoryMap(
            &MM->memory_map_size, MM->memory_map,
            &MM->map_key, &MM->descriptor_size, &MM->descriptor_version
        );

        if (EFI_BUFFER_TOO_SMALL == status){

            continue;
        }

        if (EFI_SUCCESS != status){

            error_print(L"GetMemoryMap() failed.\n", &status);
        }

        break;
    }
}


#include "totp_device.h"

#include <lib/toolbox/path.h>
#include <lib/flipper_format/flipper_format.h>

static const char* totp_app_folder = "/any/totp";
static const char* totp_app_extension = ".totp";

//static char* totp_file_header = "Flipper TOTP";
//static uint32_t totp_magspoof_file_version = 1;

TotpDevice* totp_device_alloc() {
    TotpDevice* totp_dev = malloc(sizeof(TotpDevice));
    totp_dev->storage = furi_record_open("storage");
    totp_dev->dialogs = furi_record_open("dialogs");
    string_init(totp_dev->data);
    return totp_dev;
}

void totp_device_free(TotpDevice* totp_dev) {
    furi_assert(totp_dev);
    furi_record_close("storage");
    furi_record_close("dialogs");
    free(totp_dev);
}

// static bool totp_device_save_data(FlipperFormat* file, TotpDevice* dev) {
//     bool saved = false;

//     do {
//         if(!flipper_format_write_string(file, "Data", dev->data)) break;
//         saved = true;
//     } while(false);

//     return saved;
// }

// static bool totp_device_load_common_data(FlipperFormat* file, TotpDevice* dev) {
//     // TODO
//     return false;
// }

void totp_device_free_device_set_name(TotpDevice* dev, const char* name) {
    furi_assert(dev);

    strlcpy(dev->dev_name, name, TOTP_DEV_NAME_MAX_LEN);
}

static bool totp_device_save_file(
    TotpDevice* dev,
    const char* dev_name,
    const char* folder,
    const char* extension) {
    furi_assert(dev);

    bool saved = false;

    // TODO

    return saved;
}

bool totp_device_save(TotpDevice* dev, const char* dev_name) {
    return totp_device_save_file(dev, dev_name, totp_app_folder, totp_app_extension);
}

static bool totp_device_load_data(TotpDevice* dev, string_t path) {
    bool parsed = false;

    // TODO

    return parsed;
}

bool totp_device_load(TotpDevice* dev, const char* file_path) {
    furi_assert(dev);
    furi_assert(file_path);

    // Load data
    string_t path;
    string_init_set_str(path, file_path);

    bool dev_load = totp_device_load_data(dev, path);
    if(dev_load) {
        // Set Device Name
        path_extract_filename_no_ext(file_path, path);
        //totp_device_set_name(dev, string_get_cstr(path));
    }

    string_clear(path);
    return dev_load;
}

bool totp_file_select(TotpDevice* dev) {
    furi_assert(dev);

    // Input events and views are managed by file_select
    bool res = dialog_file_select_show(
        dev->dialogs,
        totp_app_folder,
        totp_app_extension,
        dev->file_name,
        sizeof(dev->file_name),
        dev->dev_name
    );

    if(res) {
        string_t dev_str;

        // Get key file path
        string_init_printf(dev_str, "%s/%s%s", totp_app_folder, dev->file_name, totp_app_extension);
        res = totp_device_load_data(dev, dev_str);

        //if(res) {
            //totp_device_set_name(dev, dev->file_name);
        //}

        string_clear(dev_str);
    }

    return res;
}

bool totp_device_delete(TotpDevice* dev) {
    furi_assert(dev);

    bool deleted = false;

    // TODO

    return deleted;
}

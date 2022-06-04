#pragma once

#include <stdint.h>
#include <stdbool.h>
#include <m-string.h>
#include <storage/storage.h>
#include <dialogs/dialogs.h>

#include "mifare_ultralight.h"

#define TOTP_DEV_NAME_MAX_LEN 22
#define TOTP_FILE_NAME_MAX_LEN 120

typedef struct {
    string_t data;
} TotpDeviceCommonData;

typedef struct {
    TotpDeviceCommonData totp_data;
} TotpDeviceData;

typedef struct {
    Storage* storage;
    DialogsApp* dialogs;
    TotpDeviceData dev_data;
    string_t data;
    char dev_name[TOTP_DEV_NAME_MAX_LEN + 1];
    char file_name[TOTP_FILE_NAME_MAX_LEN];
    bool shadow_file_exist;
} TotpDevice;

TotpDevice* totp_device_alloc();

void totp_device_free(TotpDevice* totp_dev);

void totp_device_free_device_set_name(TotpDevice* dev, const char* name);

bool totp_device_save(TotpDevice* dev, const char* dev_name);

bool totp_device_load(TotpDevice* dev, const char* file_path);

bool totp_file_select(TotpDevice* dev);

void totp_device_clear(TotpDevice* dev);

bool totp_device_delete(TotpDevice* dev);

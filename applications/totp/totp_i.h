#pragma once

#include "totp.h"
#include "totp_device.h"

#include <furi.h>

#include <gui/gui.h>
#include <gui/view.h>
#include <gui/view_dispatcher.h>
#include <gui/scene_manager.h>
#include <cli/cli.h>
#include <notification/notification_messages.h>

#include <gui/modules/submenu.h>
#include <gui/modules/dialog_ex.h>
#include <gui/modules/popup.h>
#include <gui/modules/text_input.h>
#include <gui/modules/byte_input.h>
#include <gui/modules/text_box.h>
#include <gui/modules/widget.h>

#include <totp/scenes/totp_scene.h>

#define TOTP_SEND_NOTIFICATION_FALSE (0UL)
#define TOTP_SEND_NOTIFICATION_TRUE (1UL)
#define TOTP_TEXT_STORE_SIZE 128
struct Totp {
    ViewDispatcher* view_dispatcher;
    Gui* gui;
    NotificationApp* notification;
    SceneManager* scene_manager;
    
    TotpDevice* dev;
    TotpDeviceCommonData dev_edit_data;

    char text_store[TOTP_TEXT_STORE_SIZE + 1];
    string_t text_box_store;

    View* view;
    FuriThread* worker_thread;
    StreamBufferHandle_t rx_stream;

    // Common Views
    Submenu* submenu;
    DialogEx* dialog_ex;
    Popup* popup;
    TextInput* text_input;
    ByteInput* byte_input;
    TextBox* text_box;
    Widget* widget;
};

typedef enum {
    TotpViewMenu,
    TotpViewDialogEx,
    TotpViewPopup,
    TotpViewTextInput,
    TotpViewByteInput,
    TotpViewTextBox,
    TotpViewWidget,
    TotpViewBankCard,
} TotpView;

// static Totp* totp_alloc();

int32_t totp_task(void* p);

void totp_text_store_set(Totp* totp, const char* text, ...);

void totp_text_store_clear(Totp* app);

#pragma once

typedef struct Totp Totp;
#include <furi.h>
#include <m-string.h>
#include <gui/gui.h>
#include <notification/notification.h>
#include <notification/notification_messages.h>
#include <gui/elements.h>
#include <stdint.h>
#include <stream_buffer.h>
#include <furi_hal_uart.h>
#include <furi_hal_console.h>
#include <gui/view_dispatcher.h>
#include <gui/modules/dialog_ex.h>

#include <furi_hal_resources.h>
#include <furi_hal_gpio.h>
#include <furi_hal_delay.h>
#include <furi_hal.h>
#include "../gpio/gpio_item.h"



#define LINES_ON_SCREEN 6
#define COLUMNS_ON_SCREEN 21

typedef struct UartDumpModel UartDumpModel;

typedef struct {
    string_t text;
} ListElement;

struct UartDumpModel {
    ListElement* list[LINES_ON_SCREEN];
    uint8_t line;

    char last_char;
    bool escape;
};

typedef enum {
    WorkerEventReserved = (1 << 0), // Reserved for StreamBuffer internal event
    WorkerEventStop = (1 << 1),
    WorkerEventRx = (1 << 2),
} WorkerEventFlags;

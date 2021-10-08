#pragma once

#ifndef _CORE_CHAR_DEVICE_
#define _CORE_CHAR_DEVICE_

#include <common/defines.h>

typedef struct {
    char (*get)();
    void (*put)(char c);
} CharDevice;

void init_char_device();
void init_uart_char_device(CharDevice *device);

#endif

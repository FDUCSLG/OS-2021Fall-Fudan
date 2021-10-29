#pragma once

#include <common/defines.h>

typedef struct {
    char (*get)();
    void (*put)(char c);
} CharDevice;

void init_char_device();
void init_uart_char_device(CharDevice *device);

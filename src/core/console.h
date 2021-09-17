#pragma once

#ifndef _CORE_CONSOLE_H_
#define _CORE_CONSOLE_H_

#include <common/spinlock.h>
#include <common/variadic.h>
#include <core/char_device.h>

#define NEWLINE '\n'

typedef struct {
    SpinLock lock;
    CharDevice device;
} ConsoleContext;

void init_console();

NORETURN void no_return();

void puts(const char *str);
void vprintf(const char *fmt, va_list arg);
void printf(const char *fmt, ...);

NORETURN void _panic(const char *file, size_t line, const char *fmt, ...);

#define PANIC(...) _panic(__FILE__, __LINE__, __VA_ARGS__)

#endif

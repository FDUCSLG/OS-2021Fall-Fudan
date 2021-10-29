#pragma once

#include <common/variadic.h>
#include <core/char_device.h>

#define NEWLINE '\n'

void init_console();

void puts(const char *str);
void vprintf(const char *fmt, va_list arg);
void printf(const char *fmt, ...);

NORETURN void _panic(const char *file, usize line, const char *fmt, ...);

#define PANIC(...) _panic(__FILE__, __LINE__, __VA_ARGS__)

#define assert(predicate)                                                                          \
    do {                                                                                           \
        if (!(predicate))                                                                          \
            PANIC("assertion failed: \"%s\"", #predicate);                                         \
    } while (false)

#define asserts(predicate, ...)                                                                    \
    do {                                                                                           \
        if (!(predicate))                                                                          \
            PANIC("assertion failed: \"%s\". %s", #predicate, __VA_ARGS__);                        \
    } while (false)

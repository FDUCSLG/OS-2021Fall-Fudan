#pragma once

#ifndef _COMMON_TYPES_H_
#define _COMMON_TYPES_H_

typedef _Bool bool;

#define true 1
#define false 0

typedef signed char int8_t;
typedef unsigned char uint8_t;
typedef signed short int16_t;
typedef unsigned short uint16_t;
typedef signed int int32_t;
typedef unsigned int uint32_t;
typedef signed long long int64_t;
typedef unsigned long long uint64_t;

typedef int64_t ssize_t;
typedef uint64_t size_t;

/* Efficient min and max operations */
#define MIN(_a, _b)                 \
({                                  \
    typeof(_a) __a = (_a);          \
    typeof(_b) __b = (_b);          \
    __a <= __b ? __a : __b;         \
})

#define MAX(_a, _b)                 \
({                                  \
    typeof(_a) __a = (_a);          \
    typeof(_b) __b = (_b);          \
    __a >= __b ? __a : __b;         \
})

#define ROUNDDOWN(a, n)                 \
({                                      \
    uint64_t __a = (uint64_t) (a);      \
    (typeof(a)) (__a - __a % (n));      \
})

#define ROUNDUP(a, n)                                           \
({                                                              \
    uint64_t __n = (uint64_t) (n);                              \
    (typeof(a)) (ROUNDDOWN((uint64_t) (a) + __n - 1, __n));     \
})

// this is compatible with C++: <https://en.cppreference.com/w/c/types/NULL>.
#define NULL 0

#define NORETURN _Noreturn

#endif

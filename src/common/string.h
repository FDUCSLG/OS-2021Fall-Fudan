#pragma once

#ifndef _COMMON_STRING_H_
#define _COMMON_STRING_H_

#include <common/defines.h>

void *memset(void *s, int c, size_t n);
void *memcpy(void *restrict dest, const void *restrict src, size_t n);
int memcmp(const void *s1, const void *s2, size_t n);
void *memmove(void *dst, const void *src, size_t n);
// note: for string functions, please specify `n` explicitly.

// strncpy will `dest` with zeroes if the length of `src` is less than `n`.
// strncpy_fast will not do that.
char *strncpy(char *restrict dest, const char *restrict src, usize n);
char *strncpy_fast(char *restrict dest, const char *restrict src, usize n);

int strncmp(const char *s1, const char *s2, usize n);

usize strlen(const char *s);

#endif

extern "C" {
#include <common/defines.h>
}

#include <cstdarg>
#include <cstdint>
#include <cstdio>

#include <sstream>

#include "../exception.hpp"

extern "C" {
void _panic(const char *file, usize line, const char *fmt, ...) {
    va_list args;
    va_start(args, fmt);
    printf("(fatal) ");
    vprintf(fmt, args);
    va_end(args);
    puts("");

    std::stringstream buf;
    buf << file << ":L" << line << ": kernel panic";
    throw Panic(buf.str());
}
}

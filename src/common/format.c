#include <common/format.h>
#include <common/string.h>
#include <common/types.h>

static void _print_int(PutCharFunc put_char, void *ctx, int64_t u, int base, bool is_signed) {
    static char digit[] = "0123456789abcdef";
    static char buf[64];

    uint64_t v = u;
    if (is_signed && u < 0) {
        v = -v;
        put_char(ctx, '-');
    }

    char *pos = buf;
    do {
        *pos++ = digit[v % base];
    } while (v /= base);

    do {
        put_char(ctx, *(--pos));
    } while (pos != buf);
}

void vformat(PutCharFunc put_char, void *ctx, const char *fmt, va_list arg) {
    const char *pos = fmt;

#define _INT_CASE(ident, type, base, sign)                                                         \
    else if (strncmp(pos, ident, sizeof(ident) - 1) == 0) {                                        \
        _print_int(put_char, ctx, va_arg(arg, type), base, sign);                                  \
        pos += sizeof(ident) - 1;                                                                  \
    }

    char c;
    while ((c = *pos++) != '\0') {
        bool special = false;

        if (c == '%') {
            special = 1;

            if (*pos == '%') {
                // simple case: %% -> %
                put_char(ctx, '%');
                pos++;
            } else if (*pos == 'c') {
                put_char(ctx, va_arg(arg, int));
                pos++;
            } else if (*pos == 's') {
                const char *s = va_arg(arg, const char *);

                if (!s)
                    s = "(null)";
                while (*s != '\0') {
                    put_char(ctx, *s++);
                }

                pos++;
            }
            _INT_CASE("u", uint32_t, 10, 0)
            _INT_CASE("llu", uint64_t, 10, 0)
            _INT_CASE("d", int32_t, 10, 1)
            _INT_CASE("lld", int64_t, 10, 1)
            _INT_CASE("x", uint32_t, 16, 0)
            _INT_CASE("llx", uint64_t, 16, 0)
            _INT_CASE("p", uint64_t, 16, 0)
            _INT_CASE("zu", size_t, 10, 0)
            _INT_CASE("zd", ssize_t, 10, 1)
            else {
                special = 0;
            }
        }

        if (!special)
            put_char(ctx, c);
    }

#undef _INT_CASE
}

void format(PutCharFunc put_char, void *ctx, const char *fmt, ...) {
    va_list arg;
    va_start(arg, fmt);
    vformat(put_char, ctx, fmt, arg);
    va_end(arg);
}

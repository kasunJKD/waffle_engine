#include "debug.h"
#include <cstdio>

void debug_log(const char* file, int line, const char* fmt, ...) {
#ifdef DEBUG_ENABLED
    va_list args;
    va_start(args, fmt);
    printf("[LOG] %s:%d: ", file, line);
    vprintf(fmt, args);
    printf("\n");
    va_end(args);
#endif
}

void debug_warn(const char* file, int line, const char* fmt, ...) {
#ifdef DEBUG_ENABLED
    va_list args;
    va_start(args, fmt);
    printf("[WARN] %s:%d: ", file, line);
    vprintf(fmt, args);
    printf("\n");
    va_end(args);
#endif
}

void debug_error(const char* file, int line, const char* fmt, ...) {
#ifdef DEBUG_ENABLED
    va_list args;
    va_start(args, fmt);
    printf("[ERROR] %s:%d: ", file, line);
    vprintf(fmt, args);
    printf("\n");
    va_end(args);
#endif
}

void debug_assert_failed(const char* file, int line, const char* expr) {
#ifdef DEBUG_ENABLED
    printf("[ASSERT FAILED] %s:%d: Assertion `%s` failed.\n", file, line, expr);
#endif
}

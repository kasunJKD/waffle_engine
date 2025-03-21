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

void debug_assert_failed(const char* file, int line, const char* expr, const char* msg, ...) {
    fprintf(stderr, "[ASSERT FAILED] %s:%d: Expression (%s) failed.\n", file, line, expr);
    if (msg) {
        fprintf(stderr, "Message: ");
        va_list args;
        va_start(args, msg);
        vfprintf(stderr, msg, args);
        va_end(args);
        fprintf(stderr, "\n");
    }
}

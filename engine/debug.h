#ifndef DEBUG_H
#define DEBUG_H

#include <assert.h>
#include <stdarg.h>

#ifdef __cplusplus
extern "C" {
#endif

/* 
   To enable debug messages and assertions, define DEBUG_ENABLED
   (for example, add -DDEBUG_ENABLED to your compiler flags).
*/

#ifdef DEBUG_ENABLED
    #define DEBUG_LOG(fmt, ...)   debug_log(__FILE__, __LINE__, fmt, ##__VA_ARGS__)
    #define DEBUG_WARN(fmt, ...)  debug_warn(__FILE__, __LINE__, fmt, ##__VA_ARGS__)
    #define DEBUG_ERROR(fmt, ...) debug_error(__FILE__, __LINE__, fmt, ##__VA_ARGS__)
    #define DEBUG_ASSERT(expr, msg, ...) \
    do { \
        if (!(expr)) { \
            debug_assert_failed(__FILE__, __LINE__, #expr, msg, ##__VA_ARGS__); \
            assert(expr); \
        } \
    } while (0)
#else
    #define DEBUG_LOG(fmt, ...)   ((void)0)
    #define DEBUG_WARN(fmt, ...)  ((void)0)
    #define DEBUG_ERROR(fmt, ...) ((void)0)
    #define DEBUG_ASSERT(expr)    ((void)0)
#endif

// Function declarations for centralized debug logging.
void debug_log(const char* file, int line, const char* fmt, ...);
void debug_warn(const char* file, int line, const char* fmt, ...);
void debug_error(const char* file, int line, const char* fmt, ...);
void debug_assert_failed(const char* file, int line, const char* expr, const char* msg, ...);

#ifdef __cplusplus
}
#endif

#endif // DEBUG_H

#include "allocator.h"
#include "debug.h"
#include <cstddef>  // For std::max_align_t
#include <cstdlib>  // For malloc, free
#include <cstdio>   // For fprintf, printf
#include <cstring>  // For memset

#ifdef _WIN32
    #include <malloc.h>  // Windows-specific aligned allocation
#endif

namespace MEM {

constexpr size_t ALIGNMENT = alignof(std::max_align_t);

MemoryArena* arena_create(size_t size) {
    if (size == 0) {
        DEBUG_ERROR("arena_create: Requested size is zero. Cannot create memory arena.\n");
        return nullptr;
    }

    MemoryArena* arena = static_cast<MemoryArena*>(malloc(sizeof(MemoryArena)));
    if (!arena) {
        DEBUG_ERROR("Failed to allocate MemoryArena structure.\n");
        return nullptr;
    }

    void* base = nullptr;

    // Platform-specific aligned memory allocation
    #ifdef _WIN32
        base = _aligned_malloc(size, ALIGNMENT);
    #else
        if (posix_memalign(&base, ALIGNMENT, size) != 0) {
            base = nullptr;
        }
    #endif

    if (!base) {
        DEBUG_ERROR("Failed to allocate memory for the arena.\n");
        free(arena);
        return nullptr;
    }

    memset(base, 0, size);  // Zero-initialize the allocated memory
    arena->base = base;
    arena->size = size;
    arena->offset = 0;
    arena->allocations = 0;

    return arena;
}

void arena_destroy(MemoryArena* arena) {
    if (!arena) return;

    #ifdef _WIN32
        _aligned_free(arena->base);
    #else
        free(arena->base);
    #endif

    free(arena);
}

void* arena_alloc(MemoryArena* arena, size_t size) {
    if (!arena) {
        DEBUG_ERROR("arena_alloc: Provided arena is NULL.\n");
        return nullptr;
    }

    // Align the requested size
    size_t aligned_size = (size + (ALIGNMENT - 1)) & ~(ALIGNMENT - 1);

    if (arena->offset + aligned_size > arena->size) {
        DEBUG_ERROR("arena_alloc: Not enough memory in the arena (requested %zu bytes, available %zu bytes).\n",
                    aligned_size, arena->size - arena->offset);
        return nullptr;
    }

    // Use char* instead of void* for pointer arithmetic
    char* ptr = static_cast<char*>(arena->base) + arena->offset;
    arena->offset += aligned_size;
    arena->allocations++;

    return static_cast<void*>(ptr);
}

void arena_reset(MemoryArena* arena) {
    if (!arena) {
        DEBUG_ERROR("arena_reset: Provided arena is NULL.\n");
        return;
    }
    arena->offset = 0;
    arena->allocations = 0;
}

void arena_print_stats(const MemoryArena* arena) {
    if (!arena) {
        DEBUG_ERROR("arena_print_stats: Provided arena is NULL.\n");
        return;
    }

    printf("===== Memory Arena Stats =====\n");
    printf("  Total size:   %zu bytes\n", arena->size);
    if (arena->size > 0) {
        printf("  Used memory:  %zu bytes (%.2f%%)\n",
               arena->offset, (arena->offset / (float)arena->size) * 100);
    } else {
        printf("  Used memory:  %zu bytes (N/A - size is zero)\n", arena->offset);
    }
    printf("  Allocations:  %zu\n", arena->allocations);
    printf("==============================\n");
}

} // namespace MEM


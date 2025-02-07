#include "allocator.h"
#include "debug.h"

MemoryArena* arena_create(size_t size) {
    MemoryArena* arena = (MemoryArena*)malloc(sizeof(MemoryArena));
    if (!arena) {
	DEBUG_ERROR("Failed to allocate MemoryArena structure.\n");
        fprintf(stderr, "Failed to allocate MemoryArena structure.\n");
        return NULL;
    }
    arena->base = malloc(size);
    if (!arena->base) {
	DEBUG_ERROR("failed to allocate arena memory.\n");
        fprintf(stderr, "failed to allocate arena memory.\n");
        free(arena);
        return NULL;
    }
    arena->size = size;
    arena->offset = 0;
    arena->allocations = 0;
    return arena;
}

void arena_destroy(MemoryArena* arena) {
    if (arena) {
        free(arena->base);
        free(arena);
    }
}

void* arena_alloc(MemoryArena* arena, size_t size) {
    if (!arena) {
        fprintf(stderr, "arena_alloc: Provided arena is NULL.\n");
        return NULL;
    }
    // Ensure we have enough space left.
    if (arena->offset + size > arena->size) {
        fprintf(stderr, "arena_alloc: Not enough memory in the arena (requested %zu bytes).\n", size);
        return NULL;
    }
    void* ptr = (char*)arena->base + arena->offset;
    arena->offset += size;
    arena->allocations++;
    return ptr;
}

void arena_reset(MemoryArena* arena) {
    if (!arena) return;
    arena->offset = 0;
    arena->allocations = 0;
}

void arena_print_stats(MemoryArena* arena) {
    if (!arena) return;
    printf("Arena Stats:\n");
    printf("  Total size: %zu bytes\n", arena->size);
    printf("  Used:       %zu bytes\n", arena->offset);
    printf("  Allocations: %zu\n", arena->allocations);
}

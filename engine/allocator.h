#ifndef ALLOCATOR_H
#define ALLOCATOR_H

#include <cstddef>
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
namespace MEM {
struct MemoryArena
{
	void* base;
	size_t size;
	size_t offset;
	size_t allocations;
};

MemoryArena* arena_create(size_t size);
void arena_destroy(MemoryArena* arena);
void* arena_alloc(MemoryArena* arena, size_t size);
void arena_reset(MemoryArena* arena);
void arena_print_stats(const MemoryArena* arena);
}

#endif

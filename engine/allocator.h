#ifndef ALLOCATOR_H
#define ALLOCATOR_H

#include <cstddef>
#include <cstring>
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>

bool power_of_two(uintptr_t value);
size_t align_forward_uintptr(size_t ptr, size_t align);
size_t align_forward_size(size_t ptr, size_t align);

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

//used for persistant data otherwise there can be fragmentation
namespace Pool_Allocator {

	struct Pool_Free_Node{
		Pool_Free_Node *next;
	};

	struct Pool  {
		unsigned char* buff;
		size_t buff_len;
		size_t chunk_size;

		Pool_Free_Node *head;
	};

	void pool_init(Pool *p, void *back_buffer, size_t back_buffer_length,
		size_t chunk_size, size_t chunk_alignment);
	void pool_free_all(Pool *p);
        void *pool_alloc(Pool *p);
        void pool_free(Pool *p, void *ptr);

}

//used for bump allocator (linear/arena allocation)
namespace Temp_Allocator {

	struct TempArena  {
		unsigned char *buff;
		size_t buffer_length;
		size_t curr_offset;
		size_t prev_offset;
	};

	void *temp_alloc(TempArena *t, size_t size);

}

#endif




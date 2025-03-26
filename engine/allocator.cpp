#include "allocator.h"
#include "debug.h"
#include <cassert>
#include <cstddef>  // For std::max_align_t
#include <cstdint>
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

//reference
//https://www.gingerbill.org/article/2019/02/16/memory-allocation-strategies-004/#fn:1
bool power_of_two(uintptr_t value)
{
    return (value & (value-1)) == 0;
}

size_t align_forward_uintptr(size_t ptr, size_t align) {
    size_t a, p, modulo;

    DEBUG_ASSERT(power_of_two((uintptr_t)align), "power of two");

    a = align;
    p = ptr;
    modulo = p & (a-1);
    if (modulo != 0) {
        p+=a-modulo;
    }

    return p;
}

size_t align_forward_size(size_t ptr, size_t align) {
	size_t a, p, modulo;

	assert(power_of_two((uintptr_t)align));

	a = align;
	p = ptr;
	modulo = p & (a-1);
	if (modulo != 0) {
		p += a - modulo;
	}
	return p;
}

namespace Pool_Allocator {
	void pool_init(Pool *p, void *back_buffer, size_t back_buffer_length,
		size_t chunk_size, size_t chunk_alignment)
        {
            //allign back buffer
            uintptr_t initial_start = (uintptr_t)back_buffer;
            uintptr_t start = align_forward_uintptr(initial_start, (uintptr_t)chunk_alignment);
            back_buffer_length -= (size_t)(start - initial_start);

            //align chunk size
            chunk_size = align_forward_size(chunk_size, chunk_alignment);

            DEBUG_ASSERT(chunk_size >= sizeof(Pool_Free_Node), "Chunk size is too small");
            DEBUG_ASSERT(back_buffer_length >= chunk_size, "Back buffer length is smaller than the chunk size");

            p->buff = (unsigned char *)back_buffer;
            p->buff_len = back_buffer_length;
            p->chunk_size = chunk_size;
            p->head = NULL;

            pool_free_all(p);
        }

        void *pool_alloc(Pool *p) {
            Pool_Free_Node *node = p->head;
            if(node == NULL)
            {
                DEBUG_ASSERT(0, "Pool allocator has no free memory");
                return nullptr;
            }
            
            p->head = p->head->next;
            return memset(node, 0, p->chunk_size);
        }

        void pool_free(Pool *p, void *ptr) {
            Pool_Free_Node *node;

            void *start = p->buff;
            void *end = &p->buff[p->buff_len];

            if(ptr == nullptr)
            {
                return;
            }

            if(!(start <= ptr && ptr < end)) {
                DEBUG_ASSERT(0, "Memory is out of bounds of the buffer in this pool");
                return;
            }

            node = (Pool_Free_Node *)ptr;
            node->next = p->head;
            p->head = node;
        }

        void pool_free_all(Pool *p) {
                size_t chunk_count = p->buff_len / p->chunk_size;
                size_t i;

                // Set all chunks to be free
                for (i = 0; i < chunk_count; i++) {
                        void *ptr = &p->buff[i * p->chunk_size];
                        Pool_Free_Node *node = (Pool_Free_Node *)ptr;
                        // Push free node onto thte free list
                        node->next = p->head;
                        p->head = node;
                }
        }
}

namespace Temp_Allocator {
        void temp_arena_init(TempArena *a, void *backing_buffer, size_t backing_buffer_length) {
                a->buff = (unsigned char *)backing_buffer;
                a->buffer_length = backing_buffer_length;
                a->curr_offset = 0;
                a->prev_offset = 0;
        }

        void *temp_arena_alloc_align(TempArena *a, size_t size, size_t align) {
                // Align 'curr_offset' forward to the specified alignment
                uintptr_t curr_ptr = (uintptr_t)a->buff + (uintptr_t)a->curr_offset;
                uintptr_t offset = align_forward_uintptr(curr_ptr, align);
                offset -= (uintptr_t)a->buff; // Change to relative offset

                // Check to see if the backing memory has space left
                if (offset+size <= a->buffer_length) {
                        void *ptr = &a->buff[offset];
                        a->prev_offset = offset;
                        a->curr_offset = offset+size;

                        // Zero new memory by default
                        memset(ptr, 0, size);
                        return ptr;
                }
                // Return NULL if the arena is out of memory (or handle differently)
                return NULL;
        }

        void temp_arena_free_all(TempArena*a) {
                a->curr_offset = 0;
                a->prev_offset = 0;
        }

        void *temp_arena_alloc(TempArena *a, size_t size) {
                return temp_arena_alloc_align(a, size, 2*sizeof(void *));
        }
}


#ifndef ALLOCATOR_H
#define ALLOCATOR_H

#include <cassert>
#include <cstddef>
#include <cstdint>

#define WIN32_LEAN_AND_MEAN        // Keep <windows.h> slim
#include <windows.h>
#include <memoryapi.h>

struct Arena {
    uint8_t* base;
    size_t capacity;
    size_t current_pos;
};

static inline void* os_reserve_memory(size_t sizebyte) {
    return VirtualAlloc(NULL, sizebyte, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
}


static inline void os_release_memory(void* mem) {
    VirtualFree(mem, 0, MEM_RELEASE);
}

inline Arena* ArenaAlloc(uint8_t size) {
    const size_t header = sizeof(Arena);
    const size_t requested = sizeof(header + size);
    uint8_t* block = (uint8_t*)os_reserve_memory(requested);
    assert(block && "OS failed to reserve memory");
    Arena* a = (Arena*)block;
    a->base = block + header;
    a->capacity = requested - header;
    a->current_pos = 0;
    return a;
}

#define MiB(x) ((size_t)(x) * 1024ULL * 1024ULL)
inline Arena* ArenaAllocateMB(uint64_t sizemb) {
    return ArenaAlloc(MiB(sizemb));
}

inline void* ArenaPush(Arena* a, size_t inBytes) {
    assert(inBytes <= (a->capacity - a->current_pos) && "Arena Overflow");
    void* ptr = a->base + a->current_pos;
    a->current_pos += inBytes;
    return ptr;
}

inline void ArenaDeallocate(Arena* a) {
    if (!a) return;
    os_release_memory((void*)a);
}

#define PushArray(a,type,count)       (type*)ArenaPush((a), sizeof(type)*(count))
#define PushStruct(a,type)            PushArray((a), type, 1)

#endif

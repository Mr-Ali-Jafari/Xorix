#include "types.h"

// Simple memory allocator for kernel
static uint8_t* heap_start = (uint8_t*)0x10000000; // Start at 256MB
static uint8_t* heap_current = (uint8_t*)0x10000000;
static uint32_t heap_size = 0x1000000; // 16MB heap

// Kernel operator new
void* operator new(uint32_t size)
{
    if(heap_current + size >= heap_start + heap_size)
        return 0; // Out of memory
    
    void* result = heap_current;
    heap_current += size;
    
    // Align to 4 bytes
    while((uint32_t)heap_current % 4 != 0)
        heap_current++;
    
    return result;
}

// Kernel operator new[]
void* operator new[](uint32_t size)
{
    return operator new(size);
}

// Kernel operator delete
void operator delete(void* ptr)
{
    // Simple allocator - no deallocation
    // In a real kernel, you'd implement proper memory management
}

// Kernel operator delete[]
void operator delete[](void* ptr)
{
    operator delete(ptr);
}

// Kernel operator delete with size
void operator delete(void* ptr, uint32_t size)
{
    operator delete(ptr);
}

// Kernel operator delete[] with size
void operator delete[](void* ptr, uint32_t size)
{
    operator delete(ptr);
}

// Memory functions that might be needed
extern "C" void* memset(void* dest, int val, uint32_t len)
{
    uint8_t* d = (uint8_t*)dest;
    for(uint32_t i = 0; i < len; i++)
        d[i] = val;
    return dest;
}

extern "C" void* memcpy(void* dest, const void* src, uint32_t len)
{
    uint8_t* d = (uint8_t*)dest;
    const uint8_t* s = (const uint8_t*)src;
    for(uint32_t i = 0; i < len; i++)
        d[i] = s[i];
    return dest;
}

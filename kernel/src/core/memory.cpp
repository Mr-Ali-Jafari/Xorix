#include "types.h"
static uint8_t* heap_start = (uint8_t*)0x10000000; 
static uint8_t* heap_current = (uint8_t*)0x10000000;
static uint32_t heap_size = 0x1000000; 
void* operator new(unsigned long size)
{
    if(heap_current + size >= heap_start + heap_size)
        return 0; 
    void* result = heap_current;
    heap_current += size;
    while(((unsigned long)heap_current) % 4 != 0)
        heap_current++;
    return result;
}
void* operator new[](unsigned long size)
{
    return operator new(size);
}
void operator delete(void* ptr)
{
}
void operator delete[](void* ptr)
{
}
void operator delete(void* ptr, unsigned long size)
{
}
void operator delete[](void* ptr, unsigned long size)
{
}
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

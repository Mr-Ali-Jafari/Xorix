#ifndef MEMORY_H
#define MEMORY_H
#include "types.h"
#define HEAP_START 0x100000
#define HEAP_SIZE 0x100000
#define PAGE_SIZE 4096
void* kmalloc(size_t size);
void kfree(void* ptr);
void* krealloc(void* ptr, size_t size);
void* memset(void* dest, int val, size_t count);
void* memcpy(void* dest, const void* src, size_t count);
int memcmp(const void* ptr1, const void* ptr2, size_t count);
size_t strlen(const char* str);
char* strcpy(char* dest, const char* src);
char* strncpy(char* dest, const char* src, size_t count);
int strcmp(const char* str1, const char* str2);
int strncmp(const char* str1, const char* str2, size_t count);
char* strcat(char* dest, const char* src);
char* strstr(const char* haystack, const char* needle);
#endif 

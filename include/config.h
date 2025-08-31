#ifndef CONFIG_H
#define CONFIG_H
#define XORIX_VERSION_MAJOR 1
#define XORIX_VERSION_MINOR 0
#define XORIX_VERSION_PATCH 0
#define XORIX_VERSION_STRING "1.0.0"
#define XORIX_EDITION "Server Edition"
#define XORIX_BUILD_DATE __DATE__
#define XORIX_BUILD_TIME __TIME__
#define MAX_PROCESSES 64
#define MAX_OPEN_FILES 256
#define MAX_MEMORY_BLOCKS 1024
#define STACK_SIZE 8192
#define ENABLE_XNANO_EDITOR 1
#define ENABLE_FILESYSTEM 1
#define ENABLE_SHELL_COMMANDS 1
#define ENABLE_ROOT_MODE 1
#define ENABLE_SERVER_MODE 1
#define ENABLE_TERMINAL_SCROLLING 1
#ifdef DEBUG
#define DEBUG_PRINT(x) terminal_writestring(x)
#else
#define DEBUG_PRINT(x)
#endif
#ifdef __i386__
#define ARCH_NAME "x86"
#define ARCH_BITS 32
#elif defined(__x86_64__)
#define ARCH_NAME "x86_64"
#define ARCH_BITS 64
#elif defined(__arm__)
#define ARCH_NAME "ARM"
#define ARCH_BITS 32
#else
#define ARCH_NAME "Unknown"
#define ARCH_BITS 32
#endif
#endif 

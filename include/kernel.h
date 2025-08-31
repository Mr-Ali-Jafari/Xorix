#ifndef KERNEL_H
#define KERNEL_H
#include "types.h"
#define VGA_WIDTH 80
#define VGA_HEIGHT 25
#define VGA_MEMORY 0xB8000
enum vga_color {
    VGA_COLOR_BLACK = 0,
    VGA_COLOR_BLUE = 1,
    VGA_COLOR_GREEN = 2,
    VGA_COLOR_CYAN = 3,
    VGA_COLOR_RED = 4,
    VGA_COLOR_MAGENTA = 5,
    VGA_COLOR_BROWN = 6,
    VGA_COLOR_LIGHT_GREY = 7,
    VGA_COLOR_DARK_GREY = 8,
    VGA_COLOR_LIGHT_BLUE = 9,
    VGA_COLOR_LIGHT_GREEN = 10,
    VGA_COLOR_LIGHT_CYAN = 11,
    VGA_COLOR_LIGHT_RED = 12,
    VGA_COLOR_LIGHT_MAGENTA = 13,
    VGA_COLOR_LIGHT_BROWN = 14,
    VGA_COLOR_WHITE = 15,
};
extern "C" void kernel_main();
void terminal_initialize();
void terminal_putchar(char c);
void terminal_writestring(const char* data);
void show_shell_prompt();
void process_command(const char* command);
void xnano_editor(const char* filename);
void xnano_display_content();
void xnano_handle_input(char key);
void xnano_save_file();
void keyboard_init();
void poll_keyboard();
void* operator new(size_t size);
void* operator new[](size_t size);
void operator delete(void* ptr);
void operator delete[](void* ptr);
void outb(uint16_t port, uint8_t val);
uint8_t inb(uint16_t port);
void delay(uint32_t count);
void show_boot_splash();
void show_loading_animation();
#endif 

#ifndef KEYBOARD_H
#define KEYBOARD_H
#include "types.h"
#define KEYBOARD_DATA_PORT 0x60
#define KEYBOARD_STATUS_PORT 0x64
#define KEY_ESC 1
#define KEY_BACKSPACE 14
#define KEY_TAB 15
#define KEY_ENTER 28
#define KEY_CTRL 29
#define KEY_SHIFT_LEFT 42
#define KEY_SHIFT_RIGHT 54
#define KEY_ALT 56
#define KEY_SPACE 57
#define KEY_CAPS_LOCK 58
#define KEY_F1 59
#define KEY_F2 60
#define KEY_F3 61
#define KEY_F4 62
#define KEY_F5 63
#define KEY_F6 64
#define KEY_F7 65
#define KEY_F8 66
#define KEY_F9 67
#define KEY_F10 68
#define KEY_NUM_LOCK 69
#define KEY_SCROLL_LOCK 70
#define KEY_HOME 71
#define KEY_UP 72
#define KEY_PAGE_UP 73
#define KEY_LEFT 75
#define KEY_RIGHT 77
#define KEY_END 79
#define KEY_DOWN 80
#define KEY_PAGE_DOWN 81
#define KEY_INSERT 82
#define KEY_DELETE 83
void keyboard_init();
void poll_keyboard();
char scancode_to_ascii(uint8_t scancode);
bool is_key_pressed(uint8_t scancode);
extern bool keyboard_initialized;
extern bool ctrl_pressed;
extern bool shift_pressed;
extern bool alt_pressed;
extern bool caps_lock;
extern bool num_lock;
extern bool scroll_lock;
extern const char scancode_to_ascii_table[128];
#endif 

#include "keyboard.h"
// #include "types.h"
// #include "interrupts.h"
// #include "port.h"
// #include "gdt.h"

KeyboardDriver::KeyboardDriver(InterruptManager* manager)
: InterruptHandler(0x21, manager),
  dataport(0x60),
  commandport(0x64)
{
    while(commandport.Read() & 0x1) {
        dataport.Read();
    }
    commandport.Write(0xAE); // start sending interrupts
    commandport.Write(0x20); // get current state   
    uint8_t status = (dataport.Read() | 1) & ~0x10;
    commandport.Write(0x60); // set state
    dataport.Write(status);

    dataport.Write(0xF4);

}

KeyboardDriver::~KeyboardDriver()
{

}

inline void printf(const char* str) {
    static uint16_t* VideoMemory = (uint16_t*)0xb8000;
    static uint16_t cursor = 0;   
    for (int i = 0; str[i] != '\0'; i++) {
        char c = str[i];
        if (c == '\n') {
            cursor = (cursor / 80 + 1) * 80; 
        } else {
            VideoMemory[cursor++] = (0x07 << 8) | c; 
        }

        if (cursor >= 80 * 25) {
            for (int j = 0; j < 80 * 24; j++) {
                VideoMemory[j] = VideoMemory[j + 80];
            }
            for (int j = 80 * 24; j < 80 * 25; j++) {
                VideoMemory[j] = (0x07 << 8) | ' ';
            }
            cursor = 80 * 24;
        }
    }
}


static const char* keymap[128] =
{
    0,  // 0x00
    "ESC", // 0x01
    "1","2","3","4","5","6","7","8","9","0","-","=","BACKSPACE", // 0x0E
    "TAB", // 0x0F
    "q","w","e","r","t","y","u","i","o","p","[","]","ENTER", // 0x1C
    "CTRL", // 0x1D
    "a","s","d","f","g","h","j","k","l",";","'","`",
    "LSHIFT", // 0x2A
    "\\","z","x","c","v","b","n","m",",",".","/",
    "RSHIFT", // 0x36
    "*", // keypad *
    "ALT", // 0x38
    "SPACE", // 0x39
    "CAPSLOCK", // 0x3A
    "F1","F2","F3","F4","F5","F6","F7","F8","F9","F10", // 0x3B–0x44
    "NUMLOCK","SCROLLLOCK", // 0x45–0x46
    "HOME","UP","PGUP","-",
    "LEFT","CENTER","RIGHT","+",
    "END","DOWN","PGDN","INS","DEL", // 0x4F–0x53
    0,0,0,
    "F11","F12", // 0x57–0x58
    0, // 0x59
    0,0,0,0,0,0,0,0,0, // 0x5A–0x62
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, // fill up to 0x7F
};


uint32_t KeyboardDriver::HandleInterrupt(uint32_t esp)
{
    uint8_t key = dataport.Read();

    if (key < 0x80) 
    {
        const char* s = keymap[key];
        if (s) {
            printf(s);
        }
    }

    return esp;
}

#include "types.h"
#include "gdt.h"
#include "interrupts.h"
#include "keyboard.h"

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



extern "C" void callConstructors() {
}

extern "C" void kernelMain(void *multiboot_structure, uint32_t)
{
    
    printf("I hate Society\n");
    GlobalDescriptorTable gdt;
    InterruptManager interrupts(&gdt);



    KeyboardDriver keyboard(&interrupts);



    interrupts.Activate();
    while (1) asm("hlt");
    
}

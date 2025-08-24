#include "types.h"
#include "gdt.h"
#include "interrupts.h"

void printf(char* str){
    char* VideoMemory = (char*)0xb8000;
    for (int i = 0; str[i] != '\0'; i++)
    {
        VideoMemory[i * 2] = str[i];
        VideoMemory[i * 2 + 1] = 0x07;
    }
}


extern "C" void callConstructors() {
}

extern "C" void kernelMain(void *multiboot_structure, uint32_t)
{
    printf((char *)"Fuck Society - Fsociety / you dont know about those people , you just see them");
    GlobalDescriptorTable gdt;
    InterruptManager interrupts(&gdt);
    interrupts.Activate();
}

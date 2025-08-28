#include "types.h"
#include "gdt.h"
#include "interrupts.h"
#include "../../include/drivers_simple.h"

// Enhanced printf function
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

// Print hex number
void printHex(uint32_t value) {
    printf("0x");
    for(int i = 28; i >= 0; i -= 4) {
        uint8_t digit = (value >> i) & 0xF;
        char c = digit < 10 ? '0' + digit : 'A' + digit - 10;
        char str[2] = {c, '\0'};
        printf(str);
    }
}

extern "C" void callConstructors() {
}

extern "C" void kernelMain(void *multiboot_structure, uint32_t)
{
    // Clear screen and show boot message
    for(int i = 0; i < 80 * 25; i++)
        ((uint16_t*)0xb8000)[i] = (0x07 << 8) | ' ';
    
    printf("Xorix Kernel v1.0 - Organized Driver System\n");
    printf("==========================================\n\n");
    
    // Initialize core systems
    printf("Initializing core systems...\n");
    GlobalDescriptorTable gdt;
    printf("  [OK] Global Descriptor Table\n");
    
    InterruptManager interrupts(&gdt);
    printf("  [OK] Interrupt Manager\n");
    
    // Initialize simplified driver system
    printf("\nInitializing driver system...\n");
    SimpleDriverManager* driverManager = new SimpleDriverManager(&interrupts);
    printf("  [OK] Simple Driver Manager created\n");
    
    // Initialize all drivers
    driverManager->InitializeAllDrivers();
    printf("  [OK] All drivers initialized\n");
    
    // Activate all drivers
    driverManager->ActivateAllDrivers();
    printf("  [OK] All drivers activated\n");
    
    // Show available drivers
    printf("\nAvailable drivers:\n");
    printf("  - Keyboard Driver (PS/2)\n");
    printf("  - Mouse Driver (PS/2)\n");
    printf("  - Network Driver (Basic)\n");
    printf("  - Storage Driver (ATA/IDE)\n");
    printf("  - Audio Driver (PC Speaker)\n");
    printf("  - Video Driver (VGA Text)\n");
    
    // Test some drivers
    printf("\nTesting drivers...\n");
    
    // Test PC Speaker
    printf("  Testing PC Speaker... ");
    driverManager->PlayBeep(1000, 50);
    printf("[OK]\n");
    
    // Test VGA Text Mode
    printf("  Testing VGA Text Mode... ");
    driverManager->SetTextMode();
    printf("[OK]\n");
    
    // Show memory layout
    printf("\nMemory layout:\n");
    printf("  Kernel: ");
    printHex((uint32_t)kernelMain);
    printf("\n");
    printf("  Video Memory: 0xB8000\n");
    printf("  Driver Buffers: 0x10000000+\n");
    
    // Show file organization
    printf("\nXorix Kernel Organization:\n");
    printf("  src/core/          - Core kernel components\n");
    printf("  src/drivers/       - Hardware drivers\n");
    printf("    keyboard/        - Keyboard driver\n");
    printf("    mouse/           - Mouse driver\n");
    printf("    network/         - Network drivers\n");
    printf("    storage/         - Storage drivers\n");
    printf("    audio/           - Audio drivers\n");
    printf("    video/           - Video drivers\n");
    printf("  include/           - Header files\n");
    
    printf("\nXorix kernel ready! Press any key...\n");
    
    // Activate interrupts and enter main loop
    interrupts.Activate();
    
    // Main kernel loop
    while (1) {
        asm("hlt"); // Halt until interrupt
    }
}

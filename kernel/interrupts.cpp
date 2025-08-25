#include "interrupts.h"



InterruptHandler::InterruptHandler(uint8_t interruptNumber, InterruptManager* interruptManager)
{
    this->interruptNumber = interruptNumber;
    this->interruptManager = interruptManager;
    interruptManager->handlers[interruptNumber] = this;
}
InterruptHandler::~InterruptHandler()
{
    if (interruptManager->handlers[interruptNumber] == this)
        interruptManager->handlers[interruptNumber] = 0;
}


uint32_t InterruptHandler::HandleInterrupt(uint32_t esp)
{
    return esp;
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


InterruptManager::GateDescriptor InterruptManager::interruptDescriptorTable[256];


InterruptManager* InterruptManager::ActiveInterruptManager = 0;

void InterruptManager::SetInterruptDescriptorTableEntry(
    uint8_t interruptNumber,
    uint16_t gdt_codeSegmentSelectorOffset,
    void (*handler)(),
    uint8_t DescriptorPrivilegeLevel,
    uint8_t DescriptorType)
{
    const uint8_t IDT_DES_PRESENT = 0x80;
    interruptDescriptorTable[interruptNumber].handlerAddressLowBits = ((uint32_t)handler) & 0xFFFF;
    interruptDescriptorTable[interruptNumber].handlerAddressHighBits = (((uint32_t)handler) >> 16) & 0xFFFF;
    interruptDescriptorTable[interruptNumber].gdt_codeSegmentSelector = gdt_codeSegmentSelectorOffset;
    interruptDescriptorTable[interruptNumber].reserved = 0;
    interruptDescriptorTable[interruptNumber].access = IDT_DES_PRESENT | DescriptorType | ((DescriptorPrivilegeLevel & 3) << 5);
}

InterruptManager::InterruptManager(GlobalDescriptorTable *gdt)
:
    picMasterCommand(0x20),
    picMasterData(0x21),
    picSlaveCommand(0xA0),
    picSlaveData(0xA1)
{
    uint16_t codeSegment = gdt->CodeSegmentSelector();
    const uint8_t IDT_INTERRUPT_GATE = 0xE;

    for (uint16_t i = 0; i < 256; i++)
    {
        handlers[i] = 0;
        SetInterruptDescriptorTableEntry(i, codeSegment, &IgnoreInterruptRequest, 0, IDT_INTERRUPT_GATE);
    }

    SetInterruptDescriptorTableEntry(0x20, codeSegment, &HandleInterruptRequest0x00, 0, IDT_INTERRUPT_GATE); // IRQ0 - timer
    SetInterruptDescriptorTableEntry(0x21, codeSegment, &HandleInterruptRequest0x01, 0, IDT_INTERRUPT_GATE); // IRQ1 - keyboard

    picMasterCommand.Write(0x11);
    picSlaveCommand.Write(0x11);
    picMasterData.Write(0x20);
    picSlaveData.Write(0x28);
    picMasterData.Write(0x04);
    picSlaveData.Write(0x02);
    picMasterData.Write(0x01);
    picSlaveData.Write(0x01);
    picMasterData.Write(0x00);
    picSlaveData.Write(0x00);

    InterruptDescriptorTablePointer idt;
    idt.size = 256 * sizeof(GateDescriptor) - 1;
    idt.base = (uint32_t)interruptDescriptorTable;

    asm volatile("lidt %0" : : "m"(idt));
}

InterruptManager::~InterruptManager() {}

void InterruptManager::Activate()
{
    if(ActiveInterruptManager != 0 ){
        ActiveInterruptManager->Deactive();
    }

    ActiveInterruptManager = this;
    asm("sti");
}


void InterruptManager::Deactive()
{
    if(ActiveInterruptManager == this ){
        ActiveInterruptManager = 0;
        asm("cli");
    }


}


uint32_t InterruptManager::handleInterrupt(uint8_t interruptNumber, uint32_t esp) {
    if(ActiveInterruptManager != 0 )
        return ActiveInterruptManager->DoHandleInterrupt(interruptNumber, esp);
    return esp;
}




uint32_t InterruptManager::DoHandleInterrupt(uint8_t interruptNumber, uint32_t esp) {

    if(handlers[interruptNumber] != 0 ){
        esp = handlers[interruptNumber]->HandleInterrupt(esp);
    } else if (interruptNumber != 0x20)
    {
        char* foo = "UNHANDLED INTERRUPT 0x00\n";
        char* hex = "0123456789ABCDEF";
        foo[22] = hex[(interruptNumber >> 4) & 0x0F];
        foo[23] = hex[(interruptNumber >> 0) & 0x0F];
        printf(foo);

    }

    if (interruptNumber >= 0x20 && interruptNumber < 0x30) {
        picMasterCommand.Write(0x20);
        if (interruptNumber >= 0x28)
            picSlaveCommand.Write(0x20);
    }

    
    return esp;
}


extern "C" uint32_t handleInterrupt(uint8_t interruptNumber, uint32_t esp) {
    return InterruptManager::handleInterrupt(interruptNumber, esp);
}
#ifndef __KEYBOARD_H
#define __KEYBOARD_H

#include "../../../include/types.h"
#include "../../../include/interrupts.h"
#include "../../../include/port.h"

class KeyboardDriver: public InterruptHandler
{
    Port8Bit dataport;
    Port8Bit commandport;
public:
    KeyboardDriver(InterruptManager* manager);
    ~KeyboardDriver();
    virtual uint32_t HandleInterrupt(uint32_t esp);
};

#endif 
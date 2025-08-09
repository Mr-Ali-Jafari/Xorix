#ifndef __KEYBOARD_H
#define __KEYBOARD_H

#include "types.h"
#include "port.h"

class KeyboardDriver
{
private:
    Port8Bit dataport;
    Port8Bit commandport;

public:
    KeyboardDriver();
    ~KeyboardDriver();

    void Activate();
    bool HasKeyPressed();
    char GetKeyPressed();

private:
    char KeycodeToChar(uint8_t keycode);
};

#endif
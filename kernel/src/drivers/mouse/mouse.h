#ifndef __XORIX__DRIVERS__MOUSE_H
#define __XORIX__DRIVERS__MOUSE_H

#include "../../../include/types.h"
#include "../../../include/port.h"
#include "../../../include/interrupts.h"

namespace xorix
{
    namespace drivers
    {
        class MouseEventHandler
        {
        public:
            MouseEventHandler();
            virtual void OnActivate();
            virtual void OnMouseDown(uint8_t button);
            virtual void OnMouseUp(uint8_t button);
            virtual void OnMouseMove(int x, int y);
        };

        class MouseDriver : public xorix::hardwarecommunication::InterruptHandler
        {
            xorix::hardwarecommunication::Port8Bit dataport;
            xorix::hardwarecommunication::Port8Bit commandport;
            
            uint8_t buffer[3];
            uint8_t offset;
            uint8_t buttons;
            
            MouseEventHandler* handler;
            int8_t x, y;
            
        public:
            MouseDriver(xorix::hardwarecommunication::InterruptManager* manager, MouseEventHandler* handler);
            ~MouseDriver();
            virtual uint32_t HandleInterrupt(uint32_t esp);
            virtual void Activate();
        };
    }
}

#endif

#include "mouse.h"

using namespace xorix::drivers;
using namespace xorix::hardwarecommunication;

MouseEventHandler::MouseEventHandler()
{
}

void MouseEventHandler::OnActivate()
{
}

void MouseEventHandler::OnMouseDown(uint8_t button)
{
}

void MouseEventHandler::OnMouseUp(uint8_t button)
{
}

void MouseEventHandler::OnMouseMove(int x, int y)
{
}

MouseDriver::MouseDriver(InterruptManager* manager, MouseEventHandler* handler)
: InterruptHandler(manager, 0x2C),
  dataport(0x60),
  commandport(0x64),
  handler(handler)
{
    offset = 0;
    buttons = 0;
    x = 0;
    y = 0;
}

MouseDriver::~MouseDriver()
{
}

void MouseDriver::Activate()
{
    // Wait for command port to be ready
    while(commandport.Read() & 0x2)
        ;
    
    // Tell the controller we want to send a command to the mouse
    commandport.Write(0xD4);
    
    // Wait for command port to be ready
    while(commandport.Read() & 0x2)
        ;
    
    // Enable mouse
    dataport.Write(0xF4);
    
    // Read acknowledge
    dataport.Read();
    
    if(handler != 0)
        handler->OnActivate();
}

uint32_t MouseDriver::HandleInterrupt(uint32_t esp)
{
    uint8_t status = commandport.Read();
    if (!(status & 0x20))
        return esp;
    
    buffer[offset] = dataport.Read();
    
    if(handler == 0)
        return esp;
    
    offset = (offset + 1) % 3;
    
    if(offset == 0)
    {
        if(buffer[1] != 0 || buffer[2] != 0)
        {
            handler->OnMouseMove((int8_t)buffer[1], -((int8_t)buffer[2]));
        }
        
        for(uint8_t i = 0; i < 3; i++)
        {
            if((buffer[0] & (0x1<<i)) != (buttons & (0x1<<i)))
            {
                if(buttons & (0x1<<i))
                    handler->OnMouseUp(i+1);
                else
                    handler->OnMouseDown(i+1);
            }
        }
        buttons = buffer[0];
    }
    
    return esp;
}

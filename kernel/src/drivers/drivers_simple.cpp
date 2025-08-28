#include "../../include/drivers_simple.h"

// Mouse Event Handler Implementation
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

// Mouse Driver Implementation
MouseDriver::MouseDriver(InterruptManager* manager, MouseEventHandler* handler)
: InterruptHandler(0x2C, manager),
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
    // Basic mouse activation
    while(commandport.Read() & 0x2);
    commandport.Write(0xD4);
    while(commandport.Read() & 0x2);
    dataport.Write(0xF4);
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

// Network Driver Implementation
NetworkDriver::NetworkDriver()
{
    for(int i = 0; i < 6; i++)
        macAddress[i] = 0x00;
    ipAddress = 0;
}

NetworkDriver::~NetworkDriver()
{
}

void NetworkDriver::Initialize()
{
    // Basic network initialization
}

void NetworkDriver::SendPacket(uint32_t dstIP, uint8_t* data, uint32_t size)
{
    // Basic packet sending stub
}

void NetworkDriver::SetMACAddress(uint8_t mac[6])
{
    for(int i = 0; i < 6; i++)
        macAddress[i] = mac[i];
}

void NetworkDriver::SetIPAddress(uint32_t ip)
{
    ipAddress = ip;
}

// ATA Device Implementation
ATADevice::ATADevice(InterruptManager* manager, bool master, uint16_t portBase)
: InterruptHandler(0x20 + (portBase == 0x1F0 ? 0x0E : 0x0F), manager),
  dataPort(portBase),
  commandPort(portBase + 0x07),
  master(master)
{
}

ATADevice::~ATADevice()
{
}

void ATADevice::Activate()
{
    // Basic ATA activation
}

uint32_t ATADevice::HandleInterrupt(uint32_t esp)
{
    // Handle ATA interrupt
    return esp;
}

void ATADevice::Read28(uint32_t sectorNum, uint8_t* data)
{
    // Basic sector read stub
}

void ATADevice::Write28(uint32_t sectorNum, uint8_t* data)
{
    // Basic sector write stub
}

// PC Speaker Implementation
PCSpeaker::PCSpeaker()
: port61(0x61), port43(0x43), port42(0x42)
{
}

PCSpeaker::~PCSpeaker()
{
    StopTone();
}

void PCSpeaker::Beep(uint32_t frequency, uint32_t duration)
{
    PlayTone(frequency);
    
    // Simple delay
    for(volatile uint32_t i = 0; i < duration * 1000; i++);
    
    StopTone();
}

void PCSpeaker::PlayTone(uint32_t frequency)
{
    uint32_t divisor = 1193180 / frequency;
    
    port43.Write(0xB6);
    port42.Write(divisor & 0xFF);
    port42.Write((divisor >> 8) & 0xFF);
    
    uint8_t tmp = port61.Read();
    if(tmp != (tmp | 3))
        port61.Write(tmp | 3);
}

void PCSpeaker::StopTone()
{
    uint8_t tmp = port61.Read() & 0xFC;
    port61.Write(tmp);
}

// VGA Text Mode Implementation
VGATextMode::VGATextMode()
{
    videoMemory = (uint16_t*)0xB8000;
    cursorX = 0;
    cursorY = 0;
    color = 0x07;
}

VGATextMode::~VGATextMode()
{
}

void VGATextMode::SetColor(uint8_t foreground, uint8_t background)
{
    color = (background << 4) | (foreground & 0x0F);
}

void VGATextMode::Clear()
{
    for(int i = 0; i < 80 * 25; i++)
        videoMemory[i] = (color << 8) | ' ';
    
    cursorX = 0;
    cursorY = 0;
}

void VGATextMode::PutChar(char c)
{
    if(c == '\n')
    {
        cursorX = 0;
        cursorY++;
    }
    else if(c >= ' ')
    {
        videoMemory[cursorY * 80 + cursorX] = (color << 8) | c;
        cursorX++;
    }
    
    if(cursorX >= 80)
    {
        cursorX = 0;
        cursorY++;
    }
    
    if(cursorY >= 25)
    {
        // Scroll
        for(int i = 0; i < 80 * 24; i++)
            videoMemory[i] = videoMemory[i + 80];
        
        for(int i = 80 * 24; i < 80 * 25; i++)
            videoMemory[i] = (color << 8) | ' ';
        
        cursorY = 24;
    }
}

void VGATextMode::Print(const char* str)
{
    for(int i = 0; str[i] != '\0'; i++)
        PutChar(str[i]);
}

// Simple Driver Manager Implementation
SimpleDriverManager::SimpleDriverManager(InterruptManager* interruptManager)
{
    mouseHandler = new MouseEventHandler();
    
    keyboard = new KeyboardDriver(interruptManager);
    mouse = new MouseDriver(interruptManager, mouseHandler);
    network = new NetworkDriver();
    storage = new ATADevice(interruptManager, true, 0x1F0);
    speaker = new PCSpeaker();
    vgaText = new VGATextMode();
}

SimpleDriverManager::~SimpleDriverManager()
{
    delete keyboard;
    delete mouse;
    delete mouseHandler;
    delete network;
    delete storage;
    delete speaker;
    delete vgaText;
}

void SimpleDriverManager::InitializeAllDrivers()
{
    if(network)
        network->Initialize();
}

void SimpleDriverManager::ActivateAllDrivers()
{
    // KeyboardDriver doesn't have Activate method - it's activated automatically
    if(mouse)
        mouse->Activate();
    if(storage)
        storage->Activate();
}

void SimpleDriverManager::PlayBeep(uint32_t frequency, uint32_t duration)
{
    if(speaker)
        speaker->Beep(frequency, duration);
}

void SimpleDriverManager::SetTextMode()
{
    if(vgaText)
    {
        vgaText->Clear();
        vgaText->SetColor(7, 0);
    }
}

void SimpleDriverManager::NetworkSend(uint32_t destIP, uint8_t* data, uint32_t size)
{
    if(network)
        network->SendPacket(destIP, data, size);
}

bool SimpleDriverManager::StorageRead(uint32_t sector, uint8_t* buffer)
{
    if(storage)
    {
        storage->Read28(sector, buffer);
        return true;
    }
    return false;
}

bool SimpleDriverManager::StorageWrite(uint32_t sector, uint8_t* buffer)
{
    if(storage)
    {
        storage->Write28(sector, buffer);
        return true;
    }
    return false;
}

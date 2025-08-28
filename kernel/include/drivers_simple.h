#ifndef __XORIX__DRIVERS_SIMPLE_H
#define __XORIX__DRIVERS_SIMPLE_H

// Include core headers
#include "types.h"
#include "port.h"
#include "interrupts.h"
#include "../src/drivers/keyboard/keyboard.h"

// Simple Mouse Driver
class MouseEventHandler
{
public:
    MouseEventHandler();
    virtual void OnActivate();
    virtual void OnMouseDown(uint8_t button);
    virtual void OnMouseUp(uint8_t button);
    virtual void OnMouseMove(int x, int y);
};

class MouseDriver : public InterruptHandler
{
    Port8Bit dataport;
    Port8Bit commandport;
    
    uint8_t buffer[3];
    uint8_t offset;
    uint8_t buttons;
    
    MouseEventHandler* handler;
    int8_t x, y;
    
public:
    MouseDriver(InterruptManager* manager, MouseEventHandler* handler);
    ~MouseDriver();
    virtual uint32_t HandleInterrupt(uint32_t esp);
    virtual void Activate();
};

// Simple Network Driver
class NetworkDriver
{
protected:
    uint8_t macAddress[6];
    uint32_t ipAddress;
    
public:
    NetworkDriver();
    virtual ~NetworkDriver();
    virtual void Initialize();
    virtual void SendPacket(uint32_t dstIP, uint8_t* data, uint32_t size);
    void SetMACAddress(uint8_t mac[6]);
    void SetIPAddress(uint32_t ip);
};

// Simple Storage Driver
class ATADevice : public InterruptHandler
{
    Port16Bit dataPort;
    Port8Bit commandPort;
    bool master;
    
public:
    ATADevice(InterruptManager* manager, bool master, uint16_t portBase);
    ~ATADevice();
    virtual uint32_t HandleInterrupt(uint32_t esp);
    virtual void Activate();
    void Read28(uint32_t sectorNum, uint8_t* data);
    void Write28(uint32_t sectorNum, uint8_t* data);
};

// Simple Audio Driver
class PCSpeaker
{
    Port8Bit port61;
    Port8Bit port43;
    Port8Bit port42;
    
public:
    PCSpeaker();
    ~PCSpeaker();
    void Beep(uint32_t frequency, uint32_t duration);
    void PlayTone(uint32_t frequency);
    void StopTone();
};

// Simple Video Driver
class VGATextMode
{
    uint16_t* videoMemory;
    uint8_t cursorX, cursorY;
    uint8_t color;
    
public:
    VGATextMode();
    ~VGATextMode();
    void SetColor(uint8_t foreground, uint8_t background);
    void Clear();
    void PutChar(char c);
    void Print(const char* str);
};

// Simple Driver Manager
class SimpleDriverManager
{
    KeyboardDriver* keyboard;
    MouseDriver* mouse;
    MouseEventHandler* mouseHandler;
    NetworkDriver* network;
    ATADevice* storage;
    PCSpeaker* speaker;
    VGATextMode* vgaText;
    
public:
    SimpleDriverManager(InterruptManager* interruptManager);
    ~SimpleDriverManager();
    
    void InitializeAllDrivers();
    void ActivateAllDrivers();
    
    // Getters
    KeyboardDriver* GetKeyboard() { return keyboard; }
    MouseDriver* GetMouse() { return mouse; }
    NetworkDriver* GetNetwork() { return network; }
    ATADevice* GetStorage() { return storage; }
    PCSpeaker* GetSpeaker() { return speaker; }
    VGATextMode* GetVGAText() { return vgaText; }
    
    // Utility functions
    void PlayBeep(uint32_t frequency = 1000, uint32_t duration = 100);
    void SetTextMode();
    void NetworkSend(uint32_t destIP, uint8_t* data, uint32_t size);
    bool StorageRead(uint32_t sector, uint8_t* buffer);
    bool StorageWrite(uint32_t sector, uint8_t* buffer);
};

#endif

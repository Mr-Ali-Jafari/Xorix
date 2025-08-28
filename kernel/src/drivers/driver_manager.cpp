#include "../../include/drivers.h"

using namespace xorix::drivers;
using namespace xorix::hardwarecommunication;

DriverManager::DriverManager(InterruptManager* interruptManager)
{
    // Initialize all drivers to null
    keyboard = 0;
    mouse = 0;
    mouseHandler = 0;
    networkCard = 0;
    rtl8139 = 0;
    e1000 = 0;
    ataController = 0;
    speaker = 0;
    soundBlaster = 0;
    ac97 = 0;
    vgaText = 0;
    vgaGraphics = 0;
    vesa = 0;
    
    // Create mouse handler
    mouseHandler = new mouse::MouseEventHandler();
    
    // Initialize drivers
    keyboard = new KeyboardDriver(interruptManager);
    mouse = new mouse::MouseDriver(interruptManager, mouseHandler);
    rtl8139 = new network::RTL8139(interruptManager, 0xC000); // Common RTL8139 I/O base
    e1000 = new network::E1000(interruptManager, 0xC020);     // Common E1000 I/O base
    networkCard = rtl8139; // Default to RTL8139
    ataController = new storage::ATAController(interruptManager);
    speaker = new audio::PCSpeaker();
    soundBlaster = new audio::SoundBlaster16(interruptManager);
    ac97 = new audio::AC97Audio(interruptManager, 0x1000, 0x1010);
    vgaText = new video::VGATextMode();
    vgaGraphics = new video::VGAGraphics();
    vesa = new video::VESADriver();
}

DriverManager::~DriverManager()
{
    delete keyboard;
    delete mouse;
    delete mouseHandler;
    delete rtl8139;
    delete e1000;
    delete ataController;
    delete speaker;
    delete soundBlaster;
    delete ac97;
    delete vgaText;
    delete vgaGraphics;
    delete vesa;
}

void DriverManager::InitializeAllDrivers()
{
    // Initialize storage first (needed for other drivers)
    if(ataController)
        ataController->Initialize();
    
    // Initialize audio drivers
    if(soundBlaster)
        soundBlaster->Initialize();
    if(ac97)
        ac97->Initialize();
    
    // Initialize network drivers
    if(rtl8139)
        rtl8139->Initialize();
    if(e1000)
        e1000->Initialize();
    
    // Initialize video drivers
    if(vesa)
        vesa->Initialize();
    
    // VGA text mode is already initialized by default
    // VGA graphics mode will be initialized when needed
}

void DriverManager::ActivateAllDrivers()
{
    // Activate input drivers
    if(keyboard)
        keyboard->Activate();
    if(mouse)
        mouse->Activate();
    
    // Activate network drivers
    if(networkCard)
        networkCard->Activate();
    
    // Activate storage drivers
    if(ataController)
        ataController->Initialize(); // ATA uses Initialize as Activate
    
    // Activate audio drivers
    if(soundBlaster)
        soundBlaster->Activate();
    if(ac97)
        ac97->Activate();
}

void DriverManager::PlayBeep(uint32_t frequency, uint32_t duration)
{
    if(speaker)
        speaker->Beep(frequency, duration);
}

void DriverManager::SetVideoMode(int mode)
{
    switch(mode)
    {
        case 0: // Text mode
            if(vgaText)
            {
                vgaText->Clear();
                vgaText->SetColor(7, 0); // White on black
            }
            break;
            
        case 1: // VGA Graphics mode
            if(vgaGraphics)
            {
                vgaGraphics->SetMode(320, 200, 8);
                vgaGraphics->Clear(0);
            }
            break;
            
        case 2: // VESA mode
            if(vesa)
            {
                vesa->SetResolution(1024, 768, 32);
                vesa->Clear(0);
            }
            break;
    }
}

void DriverManager::NetworkSendPacket(uint32_t destIP, uint8_t* data, uint32_t size)
{
    if(networkCard)
        networkCard->SendPacket(destIP, data, size);
}

bool DriverManager::StorageRead(int drive, uint32_t sector, uint8_t* buffer)
{
    if(!ataController)
        return false;
    
    storage::ATADevice* device = ataController->GetDevice(drive);
    if(!device)
        return false;
    
    device->Read28(sector, buffer, 1);
    return true;
}

bool DriverManager::StorageWrite(int drive, uint32_t sector, uint8_t* buffer)
{
    if(!ataController)
        return false;
    
    storage::ATADevice* device = ataController->GetDevice(drive);
    if(!device)
        return false;
    
    device->Write28(sector, buffer, 1);
    return true;
}

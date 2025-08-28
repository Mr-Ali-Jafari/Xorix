#ifndef __XORIX__DRIVERS_H
#define __XORIX__DRIVERS_H

// Core driver includes
#include "../src/drivers/keyboard/keyboard.h"
#include "../src/drivers/mouse/mouse.h"
#include "../src/drivers/network/network.h"
#include "../src/drivers/storage/ata.h"
#include "../src/drivers/audio/audio.h"
#include "../src/drivers/video/video.h"

namespace xorix
{
    namespace drivers
    {
        // Driver Manager Class
        class DriverManager
        {
            // Keyboard
            KeyboardDriver* keyboard;
            
            // Mouse
            mouse::MouseDriver* mouse;
            mouse::MouseEventHandler* mouseHandler;
            
            // Network
            network::NetworkDriver* networkCard;
            network::RTL8139* rtl8139;
            network::E1000* e1000;
            
            // Storage
            storage::ATAController* ataController;
            
            // Audio
            audio::PCSpeaker* speaker;
            audio::SoundBlaster16* soundBlaster;
            audio::AC97Audio* ac97;
            
            // Video
            video::VGATextMode* vgaText;
            video::VGAGraphics* vgaGraphics;
            video::VESADriver* vesa;
            
        public:
            DriverManager(xorix::hardwarecommunication::InterruptManager* interruptManager);
            ~DriverManager();
            
            void InitializeAllDrivers();
            void ActivateAllDrivers();
            
            // Keyboard
            KeyboardDriver* GetKeyboard() { return keyboard; }
            
            // Mouse
            mouse::MouseDriver* GetMouse() { return mouse; }
            
            // Network
            network::NetworkDriver* GetNetworkCard() { return networkCard; }
            
            // Storage
            storage::ATAController* GetATAController() { return ataController; }
            storage::ATADevice* GetStorageDevice(int drive) { return ataController ? ataController->GetDevice(drive) : 0; }
            
            // Audio
            audio::PCSpeaker* GetSpeaker() { return speaker; }
            audio::SoundBlaster16* GetSoundBlaster() { return soundBlaster; }
            audio::AC97Audio* GetAC97() { return ac97; }
            
            // Video
            video::VGATextMode* GetVGAText() { return vgaText; }
            video::VGAGraphics* GetVGAGraphics() { return vgaGraphics; }
            video::VESADriver* GetVESA() { return vesa; }
            
            // Utility functions
            void PlayBeep(uint32_t frequency = 1000, uint32_t duration = 100);
            void SetVideoMode(int mode); // 0=text, 1=vga graphics, 2=vesa
            void NetworkSendPacket(uint32_t destIP, uint8_t* data, uint32_t size);
            bool StorageRead(int drive, uint32_t sector, uint8_t* buffer);
            bool StorageWrite(int drive, uint32_t sector, uint8_t* buffer);
        };
    }
}

#endif

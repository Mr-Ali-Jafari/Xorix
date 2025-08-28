#ifndef __XORIX__DRIVERS__ATA_H
#define __XORIX__DRIVERS__ATA_H

#include "../../../include/types.h"
#include "../../../include/port.h"
#include "../../../include/interrupts.h"

namespace xorix
{
    namespace drivers
    {
        namespace storage
        {
            class ATADevice : public xorix::hardwarecommunication::InterruptHandler
            {
                xorix::hardwarecommunication::Port16Bit dataPort;
                xorix::hardwarecommunication::Port8Bit errorPort;
                xorix::hardwarecommunication::Port8Bit sectorCountPort;
                xorix::hardwarecommunication::Port8Bit lbaLowPort;
                xorix::hardwarecommunication::Port8Bit lbaMidPort;
                xorix::hardwarecommunication::Port8Bit lbaHiPort;
                xorix::hardwarecommunication::Port8Bit devicePort;
                xorix::hardwarecommunication::Port8Bit commandPort;
                xorix::hardwarecommunication::Port8Bit controlPort;
                
                bool master;
                uint16_t bytesPerSector;
                
            public:
                ATADevice(xorix::hardwarecommunication::InterruptManager* manager, 
                         bool master, uint16_t portBase);
                ~ATADevice();
                
                virtual uint32_t HandleInterrupt(uint32_t esp);
                virtual void Activate();
                
                void Identify();
                void Read28(uint32_t sectorNum, uint8_t* data, int count = 1);
                void Write28(uint32_t sectorNum, uint8_t* data, int count = 1);
                void Flush();
                
            private:
                void SelectDrive();
                void Wait();
                bool WaitReady();
            };
            
            class ATAController
            {
                ATADevice* primaryMaster;
                ATADevice* primarySlave;
                ATADevice* secondaryMaster;
                ATADevice* secondarySlave;
                
            public:
                ATAController(xorix::hardwarecommunication::InterruptManager* manager);
                ~ATAController();
                
                ATADevice* GetDevice(int drive);
                void Initialize();
            };
        }
    }
}

#endif

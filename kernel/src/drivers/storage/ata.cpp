#include "ata.h"

using namespace xorix::drivers::storage;
using namespace xorix::hardwarecommunication;

ATADevice::ATADevice(InterruptManager* manager, bool master, uint16_t portBase)
: InterruptHandler(manager, 0x20 + (portBase == 0x1F0 ? 0x0E : 0x0F)),
  dataPort(portBase),
  errorPort(portBase + 0x01),
  sectorCountPort(portBase + 0x02),
  lbaLowPort(portBase + 0x03),
  lbaMidPort(portBase + 0x04),
  lbaHiPort(portBase + 0x05),
  devicePort(portBase + 0x06),
  commandPort(portBase + 0x07),
  controlPort(portBase + 0x206),
  master(master),
  bytesPerSector(512)
{
}

ATADevice::~ATADevice()
{
}

void ATADevice::Identify()
{
    SelectDrive();
    
    sectorCountPort.Write(0);
    lbaLowPort.Write(0);
    lbaMidPort.Write(0);
    lbaHiPort.Write(0);
    
    commandPort.Write(0xEC); // IDENTIFY command
    
    uint8_t status = commandPort.Read();
    if(status == 0)
        return; // Drive does not exist
    
    while(((status & 0x80) == 0x80) && ((status & 0x01) != 0x01))
        status = commandPort.Read();
    
    if(status & 0x01)
        return; // Error
    
    // Read identification data
    for(int i = 0; i < 256; i++)
    {
        uint16_t data = dataPort.Read();
        // Process identification data as needed
    }
}

void ATADevice::Read28(uint32_t sectorNum, uint8_t* data, int count)
{
    if(sectorNum > 0x0FFFFFFF)
        return; // 28-bit LBA limit
    
    SelectDrive();
    
    sectorCountPort.Write(count);
    lbaLowPort.Write(sectorNum & 0xFF);
    lbaMidPort.Write((sectorNum >> 8) & 0xFF);
    lbaHiPort.Write((sectorNum >> 16) & 0xFF);
    devicePort.Write(0xE0 | (master ? 0x00 : 0x10) | ((sectorNum >> 24) & 0x0F));
    
    commandPort.Write(0x20); // READ SECTORS command
    
    for(int i = 0; i < count; i++)
    {
        if(!WaitReady())
            return;
        
        for(int j = 0; j < bytesPerSector / 2; j++)
        {
            uint16_t wdata = dataPort.Read();
            data[i * bytesPerSector + j * 2] = wdata & 0xFF;
            data[i * bytesPerSector + j * 2 + 1] = (wdata >> 8) & 0xFF;
        }
    }
}

void ATADevice::Write28(uint32_t sectorNum, uint8_t* data, int count)
{
    if(sectorNum > 0x0FFFFFFF)
        return; // 28-bit LBA limit
    
    SelectDrive();
    
    sectorCountPort.Write(count);
    lbaLowPort.Write(sectorNum & 0xFF);
    lbaMidPort.Write((sectorNum >> 8) & 0xFF);
    lbaHiPort.Write((sectorNum >> 16) & 0xFF);
    devicePort.Write(0xE0 | (master ? 0x00 : 0x10) | ((sectorNum >> 24) & 0x0F));
    
    commandPort.Write(0x30); // WRITE SECTORS command
    
    for(int i = 0; i < count; i++)
    {
        if(!WaitReady())
            return;
        
        for(int j = 0; j < bytesPerSector / 2; j++)
        {
            uint16_t wdata = data[i * bytesPerSector + j * 2] |
                           (data[i * bytesPerSector + j * 2 + 1] << 8);
            dataPort.Write(wdata);
        }
    }
    
    Flush();
}

void ATADevice::Flush()
{
    SelectDrive();
    commandPort.Write(0xE7); // FLUSH CACHE command
    WaitReady();
}

void ATADevice::SelectDrive()
{
    devicePort.Write(master ? 0xA0 : 0xB0);
    Wait();
}

void ATADevice::Wait()
{
    for(int i = 0; i < 4; i++)
        controlPort.Read();
}

bool ATADevice::WaitReady()
{
    uint8_t status;
    do {
        status = commandPort.Read();
    } while((status & 0x80) && !(status & 0x01));
    
    return !(status & 0x01);
}

uint32_t ATADevice::HandleInterrupt(uint32_t esp)
{
    // Handle ATA interrupt
    return esp;
}

void ATADevice::Activate()
{
    Identify();
}

// ATAController Implementation
ATAController::ATAController(InterruptManager* manager)
{
    primaryMaster = new ATADevice(manager, true, 0x1F0);
    primarySlave = new ATADevice(manager, false, 0x1F0);
    secondaryMaster = new ATADevice(manager, true, 0x170);
    secondarySlave = new ATADevice(manager, false, 0x170);
}

ATAController::~ATAController()
{
    delete primaryMaster;
    delete primarySlave;
    delete secondaryMaster;
    delete secondarySlave;
}

void ATAController::Initialize()
{
    primaryMaster->Activate();
    primarySlave->Activate();
    secondaryMaster->Activate();
    secondarySlave->Activate();
}

ATADevice* ATAController::GetDevice(int drive)
{
    switch(drive)
    {
        case 0: return primaryMaster;
        case 1: return primarySlave;
        case 2: return secondaryMaster;
        case 3: return secondarySlave;
        default: return 0;
    }
}

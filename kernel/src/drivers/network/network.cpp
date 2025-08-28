#include "network.h"

using namespace xorix::drivers::network;
using namespace xorix::hardwarecommunication;

// Base NetworkDriver Implementation
NetworkDriver::NetworkDriver()
{
    for(int i = 0; i < 6; i++)
        macAddress[i] = 0x00;
    ipAddress = 0;
    subnetMask = 0;
    gateway = 0;
}

NetworkDriver::~NetworkDriver()
{
}

void NetworkDriver::SetMACAddress(uint8_t mac[6])
{
    for(int i = 0; i < 6; i++)
        macAddress[i] = mac[i];
}

void NetworkDriver::GetMACAddress(uint8_t mac[6])
{
    for(int i = 0; i < 6; i++)
        mac[i] = macAddress[i];
}

void NetworkDriver::SetIPAddress(uint32_t ip, uint32_t mask, uint32_t gw)
{
    ipAddress = ip;
    subnetMask = mask;
    gateway = gw;
}

uint32_t NetworkDriver::GetIPAddress()
{
    return ipAddress;
}

void NetworkDriver::SendPacket(uint32_t dstIP, uint8_t* data, uint32_t size)
{
    // Basic IP packet sending - would need ARP resolution in full implementation
    EthernetFrame frame;
    
    // Set destination MAC (broadcast for now)
    for(int i = 0; i < 6; i++)
        frame.dstMAC[i] = 0xFF;
    
    // Set source MAC
    for(int i = 0; i < 6; i++)
        frame.srcMAC[i] = macAddress[i];
    
    frame.etherType = 0x0800; // IP
    
    // Create IP header
    IPHeader* ipHeader = (IPHeader*)frame.payload;
    ipHeader->version_ihl = 0x45; // IPv4, 20 byte header
    ipHeader->tos = 0;
    ipHeader->totalLength = size + sizeof(IPHeader);
    ipHeader->identification = 0;
    ipHeader->flags_fragOffset = 0;
    ipHeader->ttl = 64;
    ipHeader->protocol = 1; // ICMP
    ipHeader->checksum = 0;
    ipHeader->srcIP = ipAddress;
    ipHeader->dstIP = dstIP;
    
    // Copy data after IP header
    for(uint32_t i = 0; i < size; i++)
        frame.payload[sizeof(IPHeader) + i] = data[i];
    
    SendFrame(&frame, sizeof(EthernetFrame) - 1500 + sizeof(IPHeader) + size);
}

void NetworkDriver::ProcessPacket(uint8_t* data, uint32_t size)
{
    // Basic packet processing - would be expanded for full network stack
}

// RTL8139 Implementation
RTL8139::RTL8139(InterruptManager* manager, uint32_t iobase)
: InterruptHandler(manager, 0x2B),
  ioBase(iobase)
{
    currentTxBuffer = 0;
    rxBufferPos = 0;
    
    // Allocate buffers (simplified allocation)
    rxBuffer = (uint8_t*)0x10000000; // 64KB receive buffer
    for(int i = 0; i < 4; i++)
        txBuffer[i] = (uint8_t*)(0x10010000 + i * 2048); // 2KB per tx buffer
}

RTL8139::~RTL8139()
{
}

void RTL8139::Initialize()
{
    // Reset the chip
    ioBase.Write(0x52, 0x10);
    while((ioBase.Read(0x52) & 0x10) != 0);
    
    // Set receive buffer
    ioBase.Write(0x30, (uint32_t)rxBuffer);
    
    // Set transmit buffers
    for(int i = 0; i < 4; i++)
        ioBase.Write(0x20 + i * 4, (uint32_t)txBuffer[i]);
    
    // Enable receive and transmit
    ioBase.Write(0x37, 0x0C);
    
    // Configure receive
    ioBase.Write(0x44, 0x0F); // Accept all packets
    
    // Enable interrupts
    ioBase.Write(0x3C, 0x0005);
}

void RTL8139::SendFrame(EthernetFrame* frame, uint32_t size)
{
    // Copy frame to current tx buffer
    for(uint32_t i = 0; i < size; i++)
        txBuffer[currentTxBuffer][i] = ((uint8_t*)frame)[i];
    
    // Send the packet
    ioBase.Write(0x10 + currentTxBuffer * 4, size);
    
    currentTxBuffer = (currentTxBuffer + 1) % 4;
}

bool RTL8139::ReceiveFrame(EthernetFrame* frame, uint32_t* size)
{
    // Check if packet available
    uint16_t status = ioBase.Read(0x3E);
    if(!(status & 0x01))
        return false;
    
    // Read packet header
    uint16_t* header = (uint16_t*)(rxBuffer + rxBufferPos);
    uint16_t packetSize = header[1] - 4; // Subtract CRC
    
    // Copy packet data
    for(uint32_t i = 0; i < packetSize && i < sizeof(EthernetFrame); i++)
        ((uint8_t*)frame)[i] = rxBuffer[rxBufferPos + 4 + i];
    
    *size = packetSize;
    
    // Update buffer position
    rxBufferPos = (rxBufferPos + packetSize + 4 + 3) & ~3; // Align to 4 bytes
    rxBufferPos %= 65536;
    
    // Update read pointer
    ioBase.Write(0x38, rxBufferPos - 16);
    
    return true;
}

uint32_t RTL8139::HandleInterrupt(uint32_t esp)
{
    uint16_t status = ioBase.Read(0x3E);
    
    if(status & 0x01) // Receive OK
    {
        // Handle received packets
    }
    
    if(status & 0x04) // Transmit OK
    {
        // Handle transmit completion
    }
    
    // Clear interrupt status
    ioBase.Write(0x3E, status);
    
    return esp;
}

void RTL8139::Activate()
{
    Initialize();
}

// E1000 Implementation (basic structure)
E1000::E1000(InterruptManager* manager, uint32_t iobase)
: InterruptHandler(manager, 0x2B),
  ioBase(iobase)
{
    rxBuffer = (uint8_t*)0x10020000;
    txBuffer = (uint8_t*)0x10030000;
    rxDescriptors = 0x10040000;
    txDescriptors = 0x10041000;
}

E1000::~E1000()
{
}

void E1000::Initialize()
{
    // Basic E1000 initialization
    // Reset
    ioBase.Write(0x00, 0x04000000);
    
    // Wait for reset
    while(ioBase.Read(0x00) & 0x04000000);
    
    // Enable interrupts
    ioBase.Write(0xD0, 0x1F6DC);
}

void E1000::SendFrame(EthernetFrame* frame, uint32_t size)
{
    // Basic E1000 transmit implementation
}

bool E1000::ReceiveFrame(EthernetFrame* frame, uint32_t* size)
{
    // Basic E1000 receive implementation
    return false;
}

uint32_t E1000::HandleInterrupt(uint32_t esp)
{
    // Handle E1000 interrupts
    return esp;
}

void E1000::Activate()
{
    Initialize();
}

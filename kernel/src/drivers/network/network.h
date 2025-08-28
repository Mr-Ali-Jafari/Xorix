#ifndef __XORIX__DRIVERS__NETWORK_H
#define __XORIX__DRIVERS__NETWORK_H

#include "../../../include/types.h"
#include "../../../include/port.h"
#include "../../../include/interrupts.h"

namespace xorix
{
    namespace drivers
    {
        namespace network
        {
            struct EthernetFrame
            {
                uint8_t dstMAC[6];
                uint8_t srcMAC[6];
                uint16_t etherType;
                uint8_t payload[1500];
            } __attribute__((packed));

            struct IPHeader
            {
                uint8_t version_ihl;
                uint8_t tos;
                uint16_t totalLength;
                uint16_t identification;
                uint16_t flags_fragOffset;
                uint8_t ttl;
                uint8_t protocol;
                uint16_t checksum;
                uint32_t srcIP;
                uint32_t dstIP;
            } __attribute__((packed));

            class NetworkDriver
            {
            protected:
                uint8_t macAddress[6];
                uint32_t ipAddress;
                uint32_t subnetMask;
                uint32_t gateway;
                
            public:
                NetworkDriver();
                virtual ~NetworkDriver();
                
                virtual void Initialize() = 0;
                virtual void SendFrame(EthernetFrame* frame, uint32_t size) = 0;
                virtual bool ReceiveFrame(EthernetFrame* frame, uint32_t* size) = 0;
                
                void SetMACAddress(uint8_t mac[6]);
                void GetMACAddress(uint8_t mac[6]);
                void SetIPAddress(uint32_t ip, uint32_t mask, uint32_t gw);
                uint32_t GetIPAddress();
                
                virtual void SendPacket(uint32_t dstIP, uint8_t* data, uint32_t size);
                virtual void ProcessPacket(uint8_t* data, uint32_t size);
            };

            // RTL8139 Network Card Driver
            class RTL8139 : public NetworkDriver, public xorix::hardwarecommunication::InterruptHandler
            {
                xorix::hardwarecommunication::Port32Bit ioBase;
                uint8_t* rxBuffer;
                uint8_t* txBuffer[4];
                uint8_t currentTxBuffer;
                uint32_t rxBufferPos;
                
            public:
                RTL8139(xorix::hardwarecommunication::InterruptManager* manager, uint32_t iobase);
                ~RTL8139();
                
                virtual void Initialize();
                virtual void SendFrame(EthernetFrame* frame, uint32_t size);
                virtual bool ReceiveFrame(EthernetFrame* frame, uint32_t* size);
                virtual uint32_t HandleInterrupt(uint32_t esp);
                virtual void Activate();
            };

            // E1000 Network Card Driver
            class E1000 : public NetworkDriver, public xorix::hardwarecommunication::InterruptHandler
            {
                xorix::hardwarecommunication::Port32Bit ioBase;
                uint8_t* rxBuffer;
                uint8_t* txBuffer;
                uint32_t rxDescriptors;
                uint32_t txDescriptors;
                
            public:
                E1000(xorix::hardwarecommunication::InterruptManager* manager, uint32_t iobase);
                ~E1000();
                
                virtual void Initialize();
                virtual void SendFrame(EthernetFrame* frame, uint32_t size);
                virtual bool ReceiveFrame(EthernetFrame* frame, uint32_t* size);
                virtual uint32_t HandleInterrupt(uint32_t esp);
                virtual void Activate();
            };
        }
    }
}

#endif

#ifndef __XORIX__DRIVERS__AUDIO_H
#define __XORIX__DRIVERS__AUDIO_H

#include "../../../include/types.h"
#include "../../../include/port.h"
#include "../../../include/interrupts.h"

namespace xorix
{
    namespace drivers
    {
        namespace audio
        {
            // PC Speaker Driver (Basic Beep)
            class PCSpeaker
            {
                xorix::hardwarecommunication::Port8Bit port61;
                xorix::hardwarecommunication::Port8Bit port43;
                xorix::hardwarecommunication::Port8Bit port42;
                
            public:
                PCSpeaker();
                ~PCSpeaker();
                
                void Beep(uint32_t frequency, uint32_t duration);
                void PlayTone(uint32_t frequency);
                void StopTone();
            };

            // Sound Blaster 16 Driver
            class SoundBlaster16 : public xorix::hardwarecommunication::InterruptHandler
            {
                xorix::hardwarecommunication::Port8Bit mixerPort;
                xorix::hardwarecommunication::Port8Bit mixerDataPort;
                xorix::hardwarecommunication::Port8Bit resetPort;
                xorix::hardwarecommunication::Port8Bit readPort;
                xorix::hardwarecommunication::Port8Bit writePort;
                xorix::hardwarecommunication::Port8Bit readStatusPort;
                
                uint8_t* dmaBuffer;
                uint32_t bufferSize;
                bool isPlaying;
                
            public:
                SoundBlaster16(xorix::hardwarecommunication::InterruptManager* manager, uint16_t port = 0x220);
                ~SoundBlaster16();
                
                virtual uint32_t HandleInterrupt(uint32_t esp);
                virtual void Activate();
                
                bool Initialize();
                void SetVolume(uint8_t volume);
                void PlayBuffer(uint8_t* buffer, uint32_t size, uint32_t sampleRate = 22050);
                void StopPlayback();
                
            private:
                void ResetDSP();
                bool WriteDSP(uint8_t value);
                uint8_t ReadDSP();
                void SetupDMA(uint8_t* buffer, uint32_t size);
            };

            // AC97 Audio Driver
            class AC97Audio : public xorix::hardwarecommunication::InterruptHandler
            {
                xorix::hardwarecommunication::Port16Bit namBar;  // Native Audio Mixer BAR
                xorix::hardwarecommunication::Port16Bit nabmBar; // Native Audio Bus Master BAR
                
                uint8_t* pcmOutBuffer;
                uint32_t bufferSize;
                bool isInitialized;
                
            public:
                AC97Audio(xorix::hardwarecommunication::InterruptManager* manager, 
                         uint16_t nambar, uint16_t nabmbar);
                ~AC97Audio();
                
                virtual uint32_t HandleInterrupt(uint32_t esp);
                virtual void Activate();
                
                bool Initialize();
                void SetMasterVolume(uint16_t volume);
                void SetPCMVolume(uint16_t volume);
                void PlayPCM(uint8_t* buffer, uint32_t size, uint32_t sampleRate = 44100);
                void StopPCM();
                
            private:
                uint16_t ReadMixer(uint8_t reg);
                void WriteMixer(uint8_t reg, uint16_t value);
                void ResetCodec();
            };
        }
    }
}

#endif

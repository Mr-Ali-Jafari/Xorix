#include "audio.h"

using namespace xorix::drivers::audio;
using namespace xorix::hardwarecommunication;

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
    
    // Simple delay (not accurate timing)
    for(volatile uint32_t i = 0; i < duration * 1000; i++)
        ;
    
    StopTone();
}

void PCSpeaker::PlayTone(uint32_t frequency)
{
    uint32_t divisor = 1193180 / frequency;
    
    // Set PIT channel 2 to square wave mode
    port43.Write(0xB6);
    
    // Set frequency
    port42.Write(divisor & 0xFF);
    port42.Write((divisor >> 8) & 0xFF);
    
    // Enable speaker
    uint8_t tmp = port61.Read();
    if(tmp != (tmp | 3))
        port61.Write(tmp | 3);
}

void PCSpeaker::StopTone()
{
    uint8_t tmp = port61.Read() & 0xFC;
    port61.Write(tmp);
}

// Sound Blaster 16 Implementation
SoundBlaster16::SoundBlaster16(InterruptManager* manager, uint16_t port)
: InterruptHandler(manager, 0x25), // IRQ 5
  mixerPort(port + 0x04),
  mixerDataPort(port + 0x05),
  resetPort(port + 0x06),
  readPort(port + 0x0A),
  writePort(port + 0x0C),
  readStatusPort(port + 0x0E)
{
    bufferSize = 4096;
    dmaBuffer = (uint8_t*)0x10050000; // Allocate DMA buffer
    isPlaying = false;
}

SoundBlaster16::~SoundBlaster16()
{
    StopPlayback();
}

bool SoundBlaster16::Initialize()
{
    ResetDSP();
    
    // Set mixer settings
    mixerPort.Write(0x22); // Master volume
    mixerDataPort.Write(0xFF); // Max volume
    
    mixerPort.Write(0x04); // Voice volume
    mixerDataPort.Write(0xFF); // Max volume
    
    return true;
}

void SoundBlaster16::ResetDSP()
{
    resetPort.Write(1);
    
    // Wait
    for(int i = 0; i < 1000; i++)
        ;
    
    resetPort.Write(0);
    
    // Wait for ready
    while(!(readStatusPort.Read() & 0x80))
        ;
    
    // Read ready byte
    readPort.Read();
}

bool SoundBlaster16::WriteDSP(uint8_t value)
{
    int timeout = 1000;
    while((writePort.Read() & 0x80) && timeout--)
        ;
    
    if(timeout <= 0)
        return false;
    
    writePort.Write(value);
    return true;
}

uint8_t SoundBlaster16::ReadDSP()
{
    while(!(readStatusPort.Read() & 0x80))
        ;
    
    return readPort.Read();
}

void SoundBlaster16::SetVolume(uint8_t volume)
{
    mixerPort.Write(0x22);
    mixerDataPort.Write(volume);
}

void SoundBlaster16::PlayBuffer(uint8_t* buffer, uint32_t size, uint32_t sampleRate)
{
    if(isPlaying)
        StopPlayback();
    
    // Copy buffer to DMA buffer
    for(uint32_t i = 0; i < size && i < bufferSize; i++)
        dmaBuffer[i] = buffer[i];
    
    SetupDMA(dmaBuffer, size);
    
    // Set sample rate
    WriteDSP(0x40);
    WriteDSP(256 - (1000000 / sampleRate));
    
    // Start playback
    WriteDSP(0x14);
    WriteDSP((size - 1) & 0xFF);
    WriteDSP(((size - 1) >> 8) & 0xFF);
    
    isPlaying = true;
}

void SoundBlaster16::StopPlayback()
{
    if(isPlaying)
    {
        WriteDSP(0xD0); // Halt DMA
        isPlaying = false;
    }
}

void SoundBlaster16::SetupDMA(uint8_t* buffer, uint32_t size)
{
    // Basic DMA setup for channel 1
    // This would need proper DMA controller programming
}

uint32_t SoundBlaster16::HandleInterrupt(uint32_t esp)
{
    // Acknowledge interrupt
    readStatusPort.Read();
    
    if(isPlaying)
    {
        // Handle playback completion
        isPlaying = false;
    }
    
    return esp;
}

void SoundBlaster16::Activate()
{
    Initialize();
}

// AC97 Audio Implementation
AC97Audio::AC97Audio(InterruptManager* manager, uint16_t nambar, uint16_t nabmbar)
: InterruptHandler(manager, 0x25),
  namBar(nambar),
  nabmBar(nabmbar)
{
    bufferSize = 8192;
    pcmOutBuffer = (uint8_t*)0x10060000;
    isInitialized = false;
}

AC97Audio::~AC97Audio()
{
    StopPCM();
}

bool AC97Audio::Initialize()
{
    ResetCodec();
    
    // Set default volumes
    SetMasterVolume(0x0000); // Max volume
    SetPCMVolume(0x0808);    // Medium volume
    
    isInitialized = true;
    return true;
}

uint16_t AC97Audio::ReadMixer(uint8_t reg)
{
    return namBar.Read(reg);
}

void AC97Audio::WriteMixer(uint8_t reg, uint16_t value)
{
    namBar.Write(reg, value);
}

void AC97Audio::ResetCodec()
{
    WriteMixer(0x00, 0x0000); // Reset register
    
    // Wait for codec ready
    int timeout = 1000;
    while(!(ReadMixer(0x26) & 0x01) && timeout--)
        ;
}

void AC97Audio::SetMasterVolume(uint16_t volume)
{
    WriteMixer(0x02, volume);
}

void AC97Audio::SetPCMVolume(uint16_t volume)
{
    WriteMixer(0x18, volume);
}

void AC97Audio::PlayPCM(uint8_t* buffer, uint32_t size, uint32_t sampleRate)
{
    if(!isInitialized)
        return;
    
    // Copy to PCM buffer
    for(uint32_t i = 0; i < size && i < bufferSize; i++)
        pcmOutBuffer[i] = buffer[i];
    
    // Setup bus master for PCM out
    // This would need proper bus master DMA setup
}

void AC97Audio::StopPCM()
{
    // Stop PCM playback
    if(isInitialized)
    {
        // Reset PCM out
    }
}

uint32_t AC97Audio::HandleInterrupt(uint32_t esp)
{
    // Handle AC97 interrupts
    return esp;
}

void AC97Audio::Activate()
{
    Initialize();
}

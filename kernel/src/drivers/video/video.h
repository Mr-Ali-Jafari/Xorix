#ifndef __XORIX__DRIVERS__VIDEO_H
#define __XORIX__DRIVERS__VIDEO_H

#include "../../../include/types.h"
#include "../../../include/port.h"

namespace xorix
{
    namespace drivers
    {
        namespace video
        {
            // VGA Text Mode Driver
            class VGATextMode
            {
                uint16_t* videoMemory;
                uint8_t cursorX, cursorY;
                uint8_t color;
                
            public:
                VGATextMode();
                ~VGATextMode();
                
                void SetColor(uint8_t foreground, uint8_t background);
                void SetCursor(uint8_t x, uint8_t y);
                void Clear();
                void PutChar(char c);
                void Print(const char* str);
                void PrintHex(uint32_t value);
                void PrintDec(uint32_t value);
                void Scroll();
            };

            // VGA Graphics Mode Driver
            class VGAGraphics
            {
                xorix::hardwarecommunication::Port8Bit miscPort;
                xorix::hardwarecommunication::Port8Bit crtcIndexPort;
                xorix::hardwarecommunication::Port8Bit crtcDataPort;
                xorix::hardwarecommunication::Port8Bit sequencerIndexPort;
                xorix::hardwarecommunication::Port8Bit sequencerDataPort;
                xorix::hardwarecommunication::Port8Bit graphicsControllerIndexPort;
                xorix::hardwarecommunication::Port8Bit graphicsControllerDataPort;
                xorix::hardwarecommunication::Port8Bit attributeControllerIndexPort;
                xorix::hardwarecommunication::Port8Bit attributeControllerReadPort;
                xorix::hardwarecommunication::Port8Bit attributeControllerWritePort;
                xorix::hardwarecommunication::Port8Bit attributeControllerResetPort;
                
                uint8_t* frameBuffer;
                uint32_t width, height;
                uint8_t colorDepth;
                
            public:
                VGAGraphics();
                ~VGAGraphics();
                
                bool SetMode(uint32_t width, uint32_t height, uint8_t colorDepth);
                void PutPixel(uint32_t x, uint32_t y, uint8_t color);
                uint8_t GetPixel(uint32_t x, uint32_t y);
                void DrawLine(uint32_t x1, uint32_t y1, uint32_t x2, uint32_t y2, uint8_t color);
                void DrawRectangle(uint32_t x, uint32_t y, uint32_t width, uint32_t height, uint8_t color);
                void FillRectangle(uint32_t x, uint32_t y, uint32_t width, uint32_t height, uint8_t color);
                void Clear(uint8_t color = 0);
                
            private:
                void WriteRegisters(uint8_t* registers);
                uint8_t GetColorIndex(uint8_t r, uint8_t g, uint8_t b);
            };

            // VESA/VBE Driver for higher resolutions
            class VESADriver
            {
                struct VBEModeInfo
                {
                    uint16_t attributes;
                    uint8_t windowA, windowB;
                    uint16_t granularity;
                    uint16_t windowSize;
                    uint16_t segmentA, segmentB;
                    uint32_t winFuncPtr;
                    uint16_t pitch;
                    uint16_t width, height;
                    uint8_t wChar, yChar, planes, bpp, banks;
                    uint8_t memoryModel, bankSize, imagePages;
                    uint8_t reserved0;
                    uint8_t redMask, redPosition;
                    uint8_t greenMask, greenPosition;
                    uint8_t blueMask, bluePosition;
                    uint8_t reservedMask, reservedPosition;
                    uint8_t directColorAttributes;
                    uint32_t frameBuffer;
                    uint32_t offScreenMemOff;
                    uint16_t offScreenMemSize;
                    uint8_t reserved1[206];
                } __attribute__((packed));
                
                VBEModeInfo* modeInfo;
                uint8_t* frameBuffer;
                uint32_t width, height, bpp;
                uint32_t pitch;
                
            public:
                VESADriver();
                ~VESADriver();
                
                bool Initialize();
                bool SetMode(uint16_t mode);
                bool SetResolution(uint32_t width, uint32_t height, uint8_t bpp);
                
                void PutPixel(uint32_t x, uint32_t y, uint32_t color);
                uint32_t GetPixel(uint32_t x, uint32_t y);
                void DrawLine(uint32_t x1, uint32_t y1, uint32_t x2, uint32_t y2, uint32_t color);
                void DrawRectangle(uint32_t x, uint32_t y, uint32_t w, uint32_t h, uint32_t color);
                void FillRectangle(uint32_t x, uint32_t y, uint32_t w, uint32_t h, uint32_t color);
                void Clear(uint32_t color = 0);
                void CopyBuffer(uint8_t* buffer, uint32_t size);
                
                uint32_t GetWidth() { return width; }
                uint32_t GetHeight() { return height; }
                uint8_t GetBPP() { return bpp; }
                
            private:
                uint32_t RGB(uint8_t r, uint8_t g, uint8_t b);
                void GetRGB(uint32_t color, uint8_t* r, uint8_t* g, uint8_t* b);
            };
        }
    }
}

#endif

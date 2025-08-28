#include "video.h"

using namespace xorix::drivers::video;
using namespace xorix::hardwarecommunication;

// VGA Text Mode Implementation
VGATextMode::VGATextMode()
{
    videoMemory = (uint16_t*)0xB8000;
    cursorX = 0;
    cursorY = 0;
    color = 0x07; // White on black
}

VGATextMode::~VGATextMode()
{
}

void VGATextMode::SetColor(uint8_t foreground, uint8_t background)
{
    color = (background << 4) | (foreground & 0x0F);
}

void VGATextMode::SetCursor(uint8_t x, uint8_t y)
{
    cursorX = x;
    cursorY = y;
    
    if(cursorX >= 80) cursorX = 79;
    if(cursorY >= 25) cursorY = 24;
}

void VGATextMode::Clear()
{
    for(int i = 0; i < 80 * 25; i++)
        videoMemory[i] = (color << 8) | ' ';
    
    cursorX = 0;
    cursorY = 0;
}

void VGATextMode::PutChar(char c)
{
    if(c == '\n')
    {
        cursorX = 0;
        cursorY++;
    }
    else if(c == '\r')
    {
        cursorX = 0;
    }
    else if(c == '\t')
    {
        cursorX = (cursorX + 8) & ~7;
    }
    else if(c >= ' ')
    {
        videoMemory[cursorY * 80 + cursorX] = (color << 8) | c;
        cursorX++;
    }
    
    if(cursorX >= 80)
    {
        cursorX = 0;
        cursorY++;
    }
    
    if(cursorY >= 25)
        Scroll();
}

void VGATextMode::Print(const char* str)
{
    for(int i = 0; str[i] != '\0'; i++)
        PutChar(str[i]);
}

void VGATextMode::PrintHex(uint32_t value)
{
    Print("0x");
    for(int i = 28; i >= 0; i -= 4)
    {
        uint8_t digit = (value >> i) & 0xF;
        PutChar(digit < 10 ? '0' + digit : 'A' + digit - 10);
    }
}

void VGATextMode::PrintDec(uint32_t value)
{
    if(value == 0)
    {
        PutChar('0');
        return;
    }
    
    char buffer[12];
    int pos = 0;
    
    while(value > 0)
    {
        buffer[pos++] = '0' + (value % 10);
        value /= 10;
    }
    
    for(int i = pos - 1; i >= 0; i--)
        PutChar(buffer[i]);
}

void VGATextMode::Scroll()
{
    for(int i = 0; i < 80 * 24; i++)
        videoMemory[i] = videoMemory[i + 80];
    
    for(int i = 80 * 24; i < 80 * 25; i++)
        videoMemory[i] = (color << 8) | ' ';
    
    cursorY = 24;
}

// VGA Graphics Mode Implementation
VGAGraphics::VGAGraphics()
: miscPort(0x3C2),
  crtcIndexPort(0x3D4),
  crtcDataPort(0x3D5),
  sequencerIndexPort(0x3C4),
  sequencerDataPort(0x3C5),
  graphicsControllerIndexPort(0x3CE),
  graphicsControllerDataPort(0x3CF),
  attributeControllerIndexPort(0x3C0),
  attributeControllerReadPort(0x3C1),
  attributeControllerWritePort(0x3C0),
  attributeControllerResetPort(0x3DA)
{
    frameBuffer = (uint8_t*)0xA0000;
    width = 320;
    height = 200;
    colorDepth = 8;
}

VGAGraphics::~VGAGraphics()
{
}

bool VGAGraphics::SetMode(uint32_t width, uint32_t height, uint8_t colorDepth)
{
    if(width == 320 && height == 200 && colorDepth == 8)
    {
        // Mode 13h registers
        uint8_t mode13h[] = {
            // MISC
            0x63,
            // SEQ
            0x03, 0x01, 0x0F, 0x00, 0x0E,
            // CRTC
            0x5F, 0x4F, 0x50, 0x82, 0x54, 0x80, 0xBF, 0x1F,
            0x00, 0x41, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
            0x9C, 0x0E, 0x8F, 0x28, 0x40, 0x96, 0xB9, 0xA3,
            0xFF,
            // GC
            0x00, 0x00, 0x00, 0x00, 0x00, 0x40, 0x05, 0x0F,
            0xFF,
            // AC
            0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
            0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F,
            0x41, 0x00, 0x0F, 0x00, 0x00
        };
        
        WriteRegisters(mode13h);
        this->width = width;
        this->height = height;
        this->colorDepth = colorDepth;
        return true;
    }
    
    return false;
}

void VGAGraphics::WriteRegisters(uint8_t* registers)
{
    // Write MISC register
    miscPort.Write(registers[0]);
    
    // Write SEQ registers
    for(uint8_t i = 0; i < 5; i++)
    {
        sequencerIndexPort.Write(i);
        sequencerDataPort.Write(registers[1 + i]);
    }
    
    // Unlock CRTC registers
    crtcIndexPort.Write(0x03);
    crtcDataPort.Write(crtcDataPort.Read() | 0x80);
    crtcIndexPort.Write(0x11);
    crtcDataPort.Write(crtcDataPort.Read() & ~0x80);
    
    // Write CRTC registers
    for(uint8_t i = 0; i < 25; i++)
    {
        crtcIndexPort.Write(i);
        crtcDataPort.Write(registers[6 + i]);
    }
    
    // Write GC registers
    for(uint8_t i = 0; i < 9; i++)
    {
        graphicsControllerIndexPort.Write(i);
        graphicsControllerDataPort.Write(registers[31 + i]);
    }
    
    // Write AC registers
    for(uint8_t i = 0; i < 21; i++)
    {
        attributeControllerResetPort.Read();
        attributeControllerIndexPort.Write(i);
        attributeControllerWritePort.Write(registers[40 + i]);
    }
    
    attributeControllerResetPort.Read();
    attributeControllerIndexPort.Write(0x20);
}

void VGAGraphics::PutPixel(uint32_t x, uint32_t y, uint8_t color)
{
    if(x >= width || y >= height)
        return;
    
    frameBuffer[y * width + x] = color;
}

uint8_t VGAGraphics::GetPixel(uint32_t x, uint32_t y)
{
    if(x >= width || y >= height)
        return 0;
    
    return frameBuffer[y * width + x];
}

void VGAGraphics::DrawLine(uint32_t x1, uint32_t y1, uint32_t x2, uint32_t y2, uint8_t color)
{
    int dx = x2 - x1;
    int dy = y2 - y1;
    int steps = (dx > dy) ? dx : dy;
    
    if(steps < 0) steps = -steps;
    if(steps == 0) steps = 1;
    
    float xIncrement = (float)dx / steps;
    float yIncrement = (float)dy / steps;
    
    float x = x1;
    float y = y1;
    
    for(int i = 0; i <= steps; i++)
    {
        PutPixel((uint32_t)x, (uint32_t)y, color);
        x += xIncrement;
        y += yIncrement;
    }
}

void VGAGraphics::DrawRectangle(uint32_t x, uint32_t y, uint32_t width, uint32_t height, uint8_t color)
{
    DrawLine(x, y, x + width - 1, y, color);
    DrawLine(x, y, x, y + height - 1, color);
    DrawLine(x + width - 1, y, x + width - 1, y + height - 1, color);
    DrawLine(x, y + height - 1, x + width - 1, y + height - 1, color);
}

void VGAGraphics::FillRectangle(uint32_t x, uint32_t y, uint32_t width, uint32_t height, uint8_t color)
{
    for(uint32_t j = y; j < y + height && j < this->height; j++)
        for(uint32_t i = x; i < x + width && i < this->width; i++)
            PutPixel(i, j, color);
}

void VGAGraphics::Clear(uint8_t color)
{
    FillRectangle(0, 0, width, height, color);
}

uint8_t VGAGraphics::GetColorIndex(uint8_t r, uint8_t g, uint8_t b)
{
    return (r >> 5) << 5 | (g >> 5) << 2 | (b >> 6);
}

// VESA Driver Implementation (Basic structure)
VESADriver::VESADriver()
{
    modeInfo = (VBEModeInfo*)0x10070000;
    frameBuffer = 0;
    width = 0;
    height = 0;
    bpp = 0;
    pitch = 0;
}

VESADriver::~VESADriver()
{
}

bool VESADriver::Initialize()
{
    // Basic VESA initialization
    // This would require BIOS calls in a real implementation
    return false;
}

bool VESADriver::SetMode(uint16_t mode)
{
    // Set VESA mode using BIOS interrupt
    // This would require real mode BIOS calls
    return false;
}

bool VESADriver::SetResolution(uint32_t width, uint32_t height, uint8_t bpp)
{
    // Find and set appropriate VESA mode
    this->width = width;
    this->height = height;
    this->bpp = bpp;
    this->pitch = width * (bpp / 8);
    
    return false; // Not implemented without BIOS support
}

void VESADriver::PutPixel(uint32_t x, uint32_t y, uint32_t color)
{
    if(!frameBuffer || x >= width || y >= height)
        return;
    
    uint32_t offset = y * pitch + x * (bpp / 8);
    
    if(bpp == 32)
        *((uint32_t*)(frameBuffer + offset)) = color;
    else if(bpp == 24)
    {
        frameBuffer[offset] = color & 0xFF;
        frameBuffer[offset + 1] = (color >> 8) & 0xFF;
        frameBuffer[offset + 2] = (color >> 16) & 0xFF;
    }
    else if(bpp == 16)
        *((uint16_t*)(frameBuffer + offset)) = color;
}

uint32_t VESADriver::GetPixel(uint32_t x, uint32_t y)
{
    if(!frameBuffer || x >= width || y >= height)
        return 0;
    
    uint32_t offset = y * pitch + x * (bpp / 8);
    
    if(bpp == 32)
        return *((uint32_t*)(frameBuffer + offset));
    else if(bpp == 24)
        return frameBuffer[offset] | (frameBuffer[offset + 1] << 8) | (frameBuffer[offset + 2] << 16);
    else if(bpp == 16)
        return *((uint16_t*)(frameBuffer + offset));
    
    return 0;
}

void VESADriver::Clear(uint32_t color)
{
    FillRectangle(0, 0, width, height, color);
}

void VESADriver::FillRectangle(uint32_t x, uint32_t y, uint32_t w, uint32_t h, uint32_t color)
{
    for(uint32_t j = y; j < y + h && j < height; j++)
        for(uint32_t i = x; i < x + w && i < width; i++)
            PutPixel(i, j, color);
}

uint32_t VESADriver::RGB(uint8_t r, uint8_t g, uint8_t b)
{
    if(bpp == 32)
        return (r << 16) | (g << 8) | b;
    else if(bpp == 24)
        return (r << 16) | (g << 8) | b;
    else if(bpp == 16)
        return ((r >> 3) << 11) | ((g >> 2) << 5) | (b >> 3);
    
    return 0;
}

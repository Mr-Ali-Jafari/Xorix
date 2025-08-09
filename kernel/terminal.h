#ifndef __TERMINAL_H
#define __TERMINAL_H

#include "types.h"
#include "keyboard.h"

class Terminal
{
private:
    static const int SCREEN_WIDTH = 80;
    static const int SCREEN_HEIGHT = 25;
    static const int BUFFER_SIZE = 256;

    uint16_t *video_memory;
    int cursor_row;
    int cursor_col;
    uint8_t color;

    char input_buffer[BUFFER_SIZE];
    int buffer_pos;

    KeyboardDriver *keyboard;

public:
    Terminal(KeyboardDriver *kbd);
    ~Terminal();

    void Clear();
    void PrintChar(char c);
    void Print(const char *str);
    void PrintLine(const char *str);
    void Newline();
    void Backspace();
    void SetColor(uint8_t new_color);
    void UpdateCursor();
    void ScrollUp();

    void ProcessInput();
    void ExecuteCommand();
    void ShowPrompt();

private:
    void ClearInputBuffer();
    bool StringCompare(const char *str1, const char *str2);
    int StringLength(const char *str);
    void StringCopy(char *dest, const char *src);
};

#endif
#include "terminal.h"

Terminal::Terminal(KeyboardDriver *kbd)
    : video_memory((uint16_t *)0xB8000), cursor_row(0), cursor_col(0),
      color(0x07), buffer_pos(0), keyboard(kbd)
{
    for (int i = 0; i < BUFFER_SIZE; i++)
        input_buffer[i] = 0;
}

Terminal::~Terminal()
{
}

void Terminal::Clear()
{
    for (int i = 0; i < SCREEN_WIDTH * SCREEN_HEIGHT; i++)
        video_memory[i] = (color << 8) | ' ';

    cursor_row = 0;
    cursor_col = 0;
    UpdateCursor();
}

void Terminal::PrintChar(char c)
{
    if (c == '\n')
    {
        Newline();
        return;
    }

    if (c == '\t')
    {
        // Tab = 4 spaces
        int spaces = 4 - (cursor_col % 4);
        for (int i = 0; i < spaces; i++)
            PrintChar(' ');
        return;
    }

    if (cursor_col >= SCREEN_WIDTH)
        Newline();

    if (cursor_row >= SCREEN_HEIGHT)
        ScrollUp();

    video_memory[cursor_row * SCREEN_WIDTH + cursor_col] = (color << 8) | c;
    cursor_col++;

    UpdateCursor();
}

void Terminal::Print(const char *str)
{
    for (int i = 0; str[i] != '\0'; i++)
        PrintChar(str[i]);
}

void Terminal::PrintLine(const char *str)
{
    Print(str);
    Newline();
}

void Terminal::Newline()
{
    cursor_row++;
    cursor_col = 0;

    if (cursor_row >= SCREEN_HEIGHT)
        ScrollUp();

    UpdateCursor();
}

void Terminal::Backspace()
{
    if (cursor_col > 0)
    {
        cursor_col--;
        video_memory[cursor_row * SCREEN_WIDTH + cursor_col] = (color << 8) | ' ';
        UpdateCursor();
    }
    else if (cursor_row > 0)
    {
        cursor_row--;
        cursor_col = SCREEN_WIDTH - 1;

        // Find the last non-space character in the previous line
        while (cursor_col > 0 &&
               (video_memory[cursor_row * SCREEN_WIDTH + cursor_col] & 0xFF) == ' ')
            cursor_col--;

        if ((video_memory[cursor_row * SCREEN_WIDTH + cursor_col] & 0xFF) != ' ')
            cursor_col++;

        UpdateCursor();
    }
}

void Terminal::SetColor(uint8_t new_color)
{
    color = new_color;
}

void Terminal::UpdateCursor()
{
    uint16_t pos = cursor_row * SCREEN_WIDTH + cursor_col;

    // Set cursor position using VGA ports
    // This is a simplified version - in a real implementation you'd use proper port classes
}

void Terminal::ScrollUp()
{
    // Move all lines up by one
    for (int row = 0; row < SCREEN_HEIGHT - 1; row++)
    {
        for (int col = 0; col < SCREEN_WIDTH; col++)
        {
            video_memory[row * SCREEN_WIDTH + col] =
                video_memory[(row + 1) * SCREEN_WIDTH + col];
        }
    }

    // Clear the last line
    for (int col = 0; col < SCREEN_WIDTH; col++)
        video_memory[(SCREEN_HEIGHT - 1) * SCREEN_WIDTH + col] = (color << 8) | ' ';

    cursor_row = SCREEN_HEIGHT - 1;
}

void Terminal::ProcessInput()
{
    char key = keyboard->GetKeyPressed();

    if (key == 0)
        return;

    if (key == '\n')
    {
        PrintChar('\n');
        input_buffer[buffer_pos] = '\0';
        ExecuteCommand();
        ClearInputBuffer();
        ShowPrompt();
    }
    else if (key == '\b')
    {
        if (buffer_pos > 0)
        {
            buffer_pos--;
            input_buffer[buffer_pos] = '\0';
            Backspace();
        }
    }
    else if (buffer_pos < BUFFER_SIZE - 1)
    {
        input_buffer[buffer_pos] = key;
        buffer_pos++;
        PrintChar(key);
    }
}

void Terminal::ExecuteCommand()
{
    if (buffer_pos == 0)
        return;

    if (StringCompare(input_buffer, "help"))
    {
        PrintLine("Available commands:");
        PrintLine("  help    - Show this help message");
        PrintLine("  clear   - Clear the screen");
        PrintLine("  about   - Show system information");
        PrintLine("  echo    - Echo back your input");
    }
    else if (StringCompare(input_buffer, "clear"))
    {
        Clear();
    }
    else if (StringCompare(input_buffer, "about"))
    {
        PrintLine("XORIX Operating System");
        PrintLine("Version: 1.0");
        PrintLine("A simple kernel with terminal support");
        PrintLine("Visit: https://Xorix.ir");
    }
    else if (buffer_pos >= 5 &&
             input_buffer[0] == 'e' && input_buffer[1] == 'c' &&
             input_buffer[2] == 'h' && input_buffer[3] == 'o' &&
             input_buffer[4] == ' ')
    {
        // Echo command - print everything after "echo "
        for (int i = 5; i < buffer_pos; i++)
            PrintChar(input_buffer[i]);
        Newline();
    }
    else
    {
        Print("Unknown command: ");
        Print(input_buffer);
        PrintLine("");
        PrintLine("Type 'help' for available commands.");
    }
}

void Terminal::ShowPrompt()
{
    SetColor(0x0A); // Green
    Print("XORIX> ");
    SetColor(0x07); // White
}

void Terminal::ClearInputBuffer()
{
    for (int i = 0; i < BUFFER_SIZE; i++)
        input_buffer[i] = 0;
    buffer_pos = 0;
}

bool Terminal::StringCompare(const char *str1, const char *str2)
{
    int i = 0;
    while (str1[i] != '\0' && str2[i] != '\0')
    {
        if (str1[i] != str2[i])
            return false;
        i++;
    }
    return str1[i] == str2[i];
}

int Terminal::StringLength(const char *str)
{
    int len = 0;
    while (str[len] != '\0')
        len++;
    return len;
}

void Terminal::StringCopy(char *dest, const char *src)
{
    int i = 0;
    while (src[i] != '\0')
    {
        dest[i] = src[i];
        i++;
    }
    dest[i] = '\0';
}
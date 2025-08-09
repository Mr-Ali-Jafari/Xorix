#include "types.h"
#include "gdt.h"
#include "keyboard.h"
#include "terminal.h"

void clear(uint8_t color)
{
    uint16_t *VideoMemory = (uint16_t *)0xb8000;
    for (int i = 0; i < 80 * 25; i++)
        VideoMemory[i] = (color << 8) | ' ';
}

void sleep(int ticks)
{
    for (volatile int i = 0; i < ticks * 10000; i++)
        ;
}

// Enhanced frame-by-frame animation function
void printOnScreenFramed(const char *str, uint8_t color, int animation_type = 0, int speed = 5000)
{
    static uint16_t *VideoMemory = (uint16_t *)0xb8000;
    int screen_width = 80;
    int screen_height = 25;

    // Calculate total lines and prepare line information
    int total_lines = 1;
    int max_line_length = 0;

    // Count lines and find max length
    for (int i = 0; str[i] != '\0'; i++)
    {
        if (str[i] == '\n')
            total_lines++;
    }

    // Store line information
    int line_lengths[total_lines];
    int line_starts[total_lines];
    int current_line = 0;
    int current_pos = 0;

    line_starts[0] = 0;
    line_lengths[0] = 0;

    for (int i = 0; str[i] != '\0'; i++)
    {
        if (str[i] == '\n')
        {
            line_lengths[current_line] = i - line_starts[current_line];
            if (line_lengths[current_line] > max_line_length)
                max_line_length = line_lengths[current_line];

            current_line++;
            if (current_line < total_lines)
            {
                line_starts[current_line] = i + 1;
                line_lengths[current_line] = 0;
            }
        }
    }

    // Handle last line if no newline at end
    if (current_line < total_lines)
    {
        line_lengths[current_line] = current_pos;
        for (int i = line_starts[current_line]; str[i] != '\0'; i++)
            line_lengths[current_line]++;

        if (line_lengths[current_line] > max_line_length)
            max_line_length = line_lengths[current_line];
    }

    int start_row = (screen_height - total_lines) / 2;

    switch (animation_type)
    {
    case 0: // Typewriter effect
    {
        for (int line = 0; line < total_lines; line++)
        {
            int col = (screen_width - line_lengths[line]) / 2;
            int row = start_row + line;

            for (int i = 0; i < line_lengths[line]; i++)
            {
                char c = str[line_starts[line] + i];
                VideoMemory[row * screen_width + col + i] = (color << 8) | c;

                // Add blinking cursor effect
                if (i < line_lengths[line] - 1)
                {
                    VideoMemory[row * screen_width + col + i + 1] = (0x0F << 8) | '_';
                    sleep(speed);
                    VideoMemory[row * screen_width + col + i + 1] = (color << 8) | ' ';
                }
                else
                {
                    sleep(speed);
                }
            }
            sleep(speed * 2); // Pause between lines
        }
        break;
    }

    case 1: // Fade-in effect (simulated with color changes)
    {
        uint8_t fade_colors[] = {0x08, 0x07, color}; // Dark gray -> Light gray -> Final color

        for (int fade_step = 0; fade_step < 3; fade_step++)
        {
            for (int line = 0; line < total_lines; line++)
            {
                int col = (screen_width - line_lengths[line]) / 2;
                int row = start_row + line;

                for (int i = 0; i < line_lengths[line]; i++)
                {
                    char c = str[line_starts[line] + i];
                    VideoMemory[row * screen_width + col + i] = (fade_colors[fade_step] << 8) | c;
                }
            }
            sleep(speed * 10);
        }
        break;
    }

    case 2: // Line-by-line reveal
    {
        for (int line = 0; line < total_lines; line++)
        {
            int col = (screen_width - line_lengths[line]) / 2;
            int row = start_row + line;

            // Flash effect before revealing
            for (int flash = 0; flash < 3; flash++)
            {
                for (int i = 0; i < line_lengths[line]; i++)
                {
                    VideoMemory[row * screen_width + col + i] = (0x70 << 8) | ' '; // White background
                }
                sleep(speed / 2);

                for (int i = 0; i < line_lengths[line]; i++)
                {
                    VideoMemory[row * screen_width + col + i] = (color << 8) | ' ';
                }
                sleep(speed / 2);
            }

            // Reveal the actual text
            for (int i = 0; i < line_lengths[line]; i++)
            {
                char c = str[line_starts[line] + i];
                VideoMemory[row * screen_width + col + i] = (color << 8) | c;
            }

            sleep(speed * 3);
        }
        break;
    }

    case 3: // Wave effect
    {
        // First, place all text
        for (int line = 0; line < total_lines; line++)
        {
            int col = (screen_width - line_lengths[line]) / 2;
            int row = start_row + line;

            for (int i = 0; i < line_lengths[line]; i++)
            {
                char c = str[line_starts[line] + i];
                VideoMemory[row * screen_width + col + i] = (0x08 << 8) | c; // Dark
            }
        }

        // Create wave effect
        for (int wave = 0; wave < max_line_length + 5; wave++)
        {
            for (int line = 0; line < total_lines; line++)
            {
                int col = (screen_width - line_lengths[line]) / 2;
                int row = start_row + line;

                for (int i = 0; i < line_lengths[line]; i++)
                {
                    char c = str[line_starts[line] + i];

                    if (i == wave || i == wave - 1)
                    {
                        VideoMemory[row * screen_width + col + i] = (0x0F << 8) | c; // Bright
                    }
                    else if (i == wave - 2)
                    {
                        VideoMemory[row * screen_width + col + i] = (color << 8) | c; // Normal
                    }
                    else if (i < wave - 2)
                    {
                        VideoMemory[row * screen_width + col + i] = (color << 8) | c; // Normal
                    }
                }
            }
            sleep(speed / 2);
        }
        break;
    }

    default: // Original behavior
    {
        for (int line = 0; line < total_lines; line++)
        {
            int col = (screen_width - line_lengths[line]) / 2;
            int row = start_row + line;

            for (int i = 0; i < line_lengths[line]; i++)
            {
                char c = str[line_starts[line] + i];
                VideoMemory[row * screen_width + col + i] = (color << 8) | c;
                sleep(speed);
            }
        }
        break;
    }
    }
}

// Keep original function for compatibility
void printOnScreen(const char *str, uint8_t color)
{
    printOnScreenFramed(str, color, 0, 1); // Fast typewriter effect
}

typedef void (*constructor)();

extern "C" constructor start_ctors;
extern "C" constructor end_ctors;

extern "C" void callConstructors()
{
    for (constructor *i = &start_ctors; i != &end_ctors; ++i)
    {
        (*i)();
    }
}

extern "C" void kernelMain(void *multiboot_structure, uint32_t)
{
    const char *welcome_msg =
        "+--------------------------------------------------+\n"
        "|                                                  |\n"
        "|        W E L C O M E   T O  XORIX......          |\n"
        "|                                                  |\n"
        "|        Kernel OPEN-SOURCE   For EveryOne         |\n"
        "|              + https://Xorix.ir                  |\n"
        "|                                                  |\n"
        "+--------------------------------------------------+\n";

    // Show welcome message with different animations
    clear(0x00); // Black background

    // Try different animation effects:
    // 0 = Typewriter effect
    // 1 = Fade-in effect
    // 2 = Line-by-line reveal with flash
    // 3 = Wave effect

    printOnScreenFramed(welcome_msg, 0x0A, 3, 3000); // Wave effect with green color

    // Wait for a moment to display the welcome message
    sleep(300000);

    // Initialize GDT
    GlobalDescriptorTable gdt;

    // Initialize keyboard and terminal
    KeyboardDriver keyboard;
    keyboard.Activate();

    Terminal terminal(&keyboard);
    terminal.Clear();

    // Welcome to terminal with typewriter effect
    const char *terminal_msg = "XORIX Terminal Started\nType 'help' for available commands";
    printOnScreenFramed(terminal_msg, 0x0F, 0, 8000); // Typewriter effect

    sleep(100000);
    terminal.Clear();

    // Show initial prompt
    terminal.ShowPrompt();

    // Main terminal loop
    while (true)
    {
        terminal.ProcessInput();
    }
}
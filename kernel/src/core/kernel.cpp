typedef unsigned char uint8_t;
typedef unsigned short uint16_t;
typedef unsigned int uint32_t;
typedef unsigned long long uint64_t;
typedef long unsigned int size_t;
size_t strlen(const char* str) {
    size_t len = 0;
    while (str[len]) len++;
    return len;
}
int strcmp(const char* str1, const char* str2) {
    while (*str1 && (*str1 == *str2)) {
        str1++;
        str2++;
    }
    return *(unsigned char*)str1 - *(unsigned char*)str2;
}
char* strcpy(char* dest, const char* src) {
    char* original_dest = dest;
    while ((*dest++ = *src++));
    return original_dest;
}
void* memset(void* ptr, int value, size_t num) {
    unsigned char* p = (unsigned char*)ptr;
    while (num--) *p++ = (unsigned char)value;
    return ptr;
}
void* memcpy(void* dest, const void* src, size_t num) {
    unsigned char* d = (unsigned char*)dest;
    const unsigned char* s = (const unsigned char*)src;
    while (num--) *d++ = *s++;
    return dest;
}
char* strcat(char* dest, const char* src) {
    char* original_dest = dest;
    while (*dest) dest++; 
    while ((*dest++ = *src++)); 
    return original_dest;
}
char* strstr(const char* haystack, const char* needle) {
    if (!*needle) return (char*)haystack;
    for (const char* h = haystack; *h; h++) {
        const char* h_temp = h;
        const char* n_temp = needle;
        while (*h_temp && *n_temp && *h_temp == *n_temp) {
            h_temp++;
            n_temp++;
        }
        if (!*n_temp) return (char*)h;
    }
    return 0;
}
char* strncpy(char* dest, const char* src, size_t num) {
    char* original_dest = dest;
    size_t i;
    for (i = 0; i < num && src[i] != '\0'; i++) {
        dest[i] = src[i];
    }
    for (; i < num; i++) {
        dest[i] = '\0';
    }
    return original_dest;
}
void outb(uint16_t port, uint8_t val) {
    asm volatile ( "outb %0, %1" : : "a"(val), "Nd"(port) );
}
uint8_t inb(uint16_t port) {
    uint8_t ret;
    asm volatile ( "inb %1, %0" : "=a"(ret) : "Nd"(port) );
    return ret;
}
#define VGA_WIDTH 80
#define VGA_HEIGHT 25
#define VGA_MEMORY ((uint16_t*) 0xB8000)
#define VGA_COLOR_BLACK 0
#define VGA_COLOR_BLUE 1
#define VGA_COLOR_GREEN 2
#define VGA_COLOR_CYAN 3
#define VGA_COLOR_RED 4
#define VGA_COLOR_MAGENTA 5
#define VGA_COLOR_BROWN 6
#define VGA_COLOR_LIGHT_GREY 7
#define VGA_COLOR_DARK_GREY 8
#define VGA_COLOR_LIGHT_BLUE 9
#define VGA_COLOR_LIGHT_GREEN 10
#define VGA_COLOR_LIGHT_CYAN 11
#define VGA_COLOR_LIGHT_RED 12
#define VGA_COLOR_LIGHT_MAGENTA 13
#define VGA_COLOR_LIGHT_BROWN 14
#define VGA_COLOR_WHITE 15
#define VGA_COLOR_YELLOW VGA_COLOR_BROWN
#define VGA_COLOR_LIGHT_YELLOW VGA_COLOR_LIGHT_BROWN
size_t terminal_row;
size_t terminal_column;
uint8_t terminal_color;
uint16_t* terminal_buffer;
bool root_mode = false;
char current_directory[256] = "/";
char username[64] = "user";
char root_password[64] = "xorix123";
#define TERMINAL_BUFFER_SIZE 1000
char terminal_history[TERMINAL_BUFFER_SIZE][VGA_WIDTH + 1];
int terminal_history_count = 0;
int terminal_scroll_offset = 0;
bool scrolling_enabled = false;
#define MAX_FILES 100
struct XorixFile {
    char name[64];
    char path[256];
    char content[1024];
    bool is_directory;
    size_t size;
};
XorixFile filesystem[MAX_FILES];
int file_count = 0;
char input_buffer[256];
size_t input_length;
bool keyboard_initialized;
bool server_mode;
bool xnano_mode = false;
char xnano_filename[256];
char xnano_content[50][VGA_WIDTH + 1]; 
int xnano_line_count = 0;
int xnano_cursor_line = 0;
int xnano_cursor_col = 0;
int xnano_scroll_offset = 0;
bool xnano_modified = false;
const char scancode_to_ascii[128] = {
    0,  27, '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '-', '=', '\b',
    '\t', 'q', 'w', 'e', 'r', 't', 'y', 'u', 'i', 'o', 'p', '[', ']', '\n',
    0, 'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', ';', '\'', '`', 0,
    '\\', 'z', 'x', 'c', 'v', 'b', 'n', 'm', ',', '.', '/', 0, '*',
    0, ' ', 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
};
void process_command(const char* command);
void init_filesystem();
void scroll_terminal_up();
void scroll_terminal_down();
void add_to_terminal_history(const char* line);
void refresh_terminal_display();
void handle_numpad_input(char key);
void xnano_editor(const char* filename);
void xnano_display_content();
void xnano_handle_input(char key);
void xnano_save_file();
void xnano_show_help();
void show_shell_prompt();
uint16_t vga_entry(unsigned char uc, uint8_t color) {
    return (uint16_t) uc | (uint16_t) color << 8;
}
void terminal_initialize(void) {
    terminal_row = 0;
    terminal_column = 0;
    terminal_color = VGA_COLOR_LIGHT_GREY | VGA_COLOR_BLACK << 4;
    terminal_buffer = VGA_MEMORY;
    size_t y, x;
    for (y = 0; y < VGA_HEIGHT; y++) {
        for (x = 0; x < VGA_WIDTH; x++) {
            const size_t index = y * VGA_WIDTH + x;
            terminal_buffer[index] = vga_entry(' ', terminal_color);
        }
    }
}
void terminal_putentryat(char c, uint8_t color, size_t x, size_t y) {
    const size_t index = y * VGA_WIDTH + x;
    terminal_buffer[index] = vga_entry(c, color);
}
void terminal_putchar(char c) {
    if (c == '\n') {
        terminal_column = 0;
        if (++terminal_row == VGA_HEIGHT) {
            size_t y, x;
            for (y = 0; y < VGA_HEIGHT - 1; y++) {
                for (x = 0; x < VGA_WIDTH; x++) {
                    terminal_buffer[y * VGA_WIDTH + x] = terminal_buffer[(y + 1) * VGA_WIDTH + x];
                }
            }
            for (x = 0; x < VGA_WIDTH; x++) {
                terminal_buffer[(VGA_HEIGHT - 1) * VGA_WIDTH + x] = vga_entry(' ', terminal_color);
            }
            terminal_row = VGA_HEIGHT - 1;
        }
    } else {
        terminal_putentryat(c, terminal_color, terminal_column, terminal_row);
        if (++terminal_column == VGA_WIDTH) {
            terminal_column = 0;
            if (++terminal_row == VGA_HEIGHT) {
                size_t y, x;
                for (y = 0; y < VGA_HEIGHT - 1; y++) {
                    for (x = 0; x < VGA_WIDTH; x++) {
                        terminal_buffer[y * VGA_WIDTH + x] = terminal_buffer[(y + 1) * VGA_WIDTH + x];
                    }
                }
                for (x = 0; x < VGA_WIDTH; x++) {
                    terminal_buffer[(VGA_HEIGHT - 1) * VGA_WIDTH + x] = vga_entry(' ', terminal_color);
                }
                terminal_row = VGA_HEIGHT - 1;
            }
        }
    }
}
void terminal_write(const char* data, size_t size) {
    size_t i;
    for (i = 0; i < size; i++) {
        terminal_putchar(data[i]);
    }
}
void terminal_writestring(const char* data) { 
    terminal_write(data, strlen(data)); 
    if (!xnano_mode && !scrolling_enabled) {
        add_to_terminal_history(data);
    }
}
void add_to_terminal_history(const char* line) {
    if (strlen(line) < 1) return;
    if (terminal_history_count < TERMINAL_BUFFER_SIZE) {
        strncpy(terminal_history[terminal_history_count], line, VGA_WIDTH);
        terminal_history[terminal_history_count][VGA_WIDTH] = '\0';
        terminal_history_count++;
    } else {
        for (int i = 0; i < TERMINAL_BUFFER_SIZE - 1; i++) {
            strcpy(terminal_history[i], terminal_history[i + 1]);
        }
        strncpy(terminal_history[TERMINAL_BUFFER_SIZE - 1], line, VGA_WIDTH);
        terminal_history[TERMINAL_BUFFER_SIZE - 1][VGA_WIDTH] = '\0';
    }
}
void scroll_terminal_up() {
    if (!xnano_mode && terminal_history_count > VGA_HEIGHT) {
        if (terminal_scroll_offset > 0) {
            terminal_scroll_offset--;
            refresh_terminal_display();
        }
    }
}
void scroll_terminal_down() {
    if (!xnano_mode && terminal_history_count > VGA_HEIGHT) {
        int max_offset = terminal_history_count - VGA_HEIGHT;
        if (terminal_scroll_offset < max_offset) {
            terminal_scroll_offset++;
            refresh_terminal_display();
        }
    }
}
void refresh_terminal_display() {
    scrolling_enabled = true; 
    for (size_t y = 0; y < VGA_HEIGHT; y++) {
        for (size_t x = 0; x < VGA_WIDTH; x++) {
            terminal_putentryat(' ', terminal_color, x, y);
        }
    }
    terminal_row = 0;
    terminal_column = 0;
    int start_line = terminal_scroll_offset;
    int end_line = start_line + VGA_HEIGHT;
    for (int i = start_line; i < end_line && i < terminal_history_count; i++) {
        terminal_write(terminal_history[i], strlen(terminal_history[i]));
        if (terminal_row < VGA_HEIGHT - 1) {
            terminal_row++;
            terminal_column = 0;
        }
    }
    scrolling_enabled = false; 
}
void handle_numpad_input(char key) {
    switch (key) {
        case '2': 
            scroll_terminal_down();
            break;
        case '8': 
            scroll_terminal_up();
            break;
    }
}
void init_filesystem() {
    strcpy(filesystem[0].name, "/");
    strcpy(filesystem[0].path, "/");
    filesystem[0].is_directory = true;
    filesystem[0].size = 0;
    file_count = 1;
    strcpy(filesystem[1].name, "home");
    strcpy(filesystem[1].path, "/home");
    filesystem[1].is_directory = true;
    filesystem[1].size = 0;
    strcpy(filesystem[2].name, "bin");
    strcpy(filesystem[2].path, "/bin");
    filesystem[2].is_directory = true;
    filesystem[2].size = 0;
    strcpy(filesystem[3].name, "etc");
    strcpy(filesystem[3].path, "/etc");
    filesystem[3].is_directory = true;
    filesystem[3].size = 0;
    file_count = 4;
}
void keyboard_init() { 
    outb(0x64, 0xAE);
    input_length = 0; 
    keyboard_initialized = true; 
}
static bool ctrl_pressed = false;
void poll_keyboard() {
    uint8_t status = inb(0x64);
    if (!(status & 0x01)) return;
    uint8_t scancode = inb(0x60);
    if (scancode == 29) { 
        ctrl_pressed = true;
        return;
    } else if (scancode == 157) { 
        ctrl_pressed = false;
        return;
    }
    if (!(scancode & 0x80)) {
        if (xnano_mode) {
            if (scancode == 72) { 
                xnano_handle_input('U'); 
            } else if (scancode == 80) { 
                xnano_handle_input('D'); 
            } else if (scancode == 75) { 
                xnano_handle_input('L'); 
            } else if (scancode == 77) { 
                xnano_handle_input('R'); 
            } else if (scancode == 1) { 
                xnano_handle_input(27); 
            } else if (scancode < 128 && scancode_to_ascii[scancode]) {
                char ascii = scancode_to_ascii[scancode];
                if (ascii != 0) {
                    xnano_handle_input(ascii);
                }
            }
        } else {
            if (scancode == 72) { 
                scroll_terminal_up();
            } else if (scancode == 80) { 
                scroll_terminal_down();
            } else if (scancode < 128 && scancode_to_ascii[scancode]) {
                char ascii = scancode_to_ascii[scancode];
                if (ascii != 0) {
                    if (ascii == '\n') {
                        terminal_putchar('\n');
                        if (input_length > 0) {
                            input_buffer[input_length] = '\0';
                            process_command(input_buffer);
                            input_length = 0;
                        }
                        show_shell_prompt();
                    } else if (ascii == '\b') {
                        if (input_length > 0) {
                            input_length--;
                            if (terminal_column > 0) {
                                terminal_column--;
                                terminal_putentryat(' ', terminal_color, terminal_column, terminal_row);
                            }
                        }
                    } else if (input_length < sizeof(input_buffer) - 2) {
                        input_buffer[input_length++] = ascii;
                        terminal_putchar(ascii);
                    }
                }
            }
        }
    }
}
void process_command_with_style(const char* command) {
    terminal_color = VGA_COLOR_LIGHT_GREY | VGA_COLOR_BLACK << 4;
    process_command(command);
}
void process_command(const char* command) {
    if (strcmp(command, "help") == 0) {
        terminal_color = VGA_COLOR_LIGHT_CYAN | VGA_COLOR_BLACK << 4;
        terminal_writestring("Xorix Server Edition Commands:\n");
        terminal_color = VGA_COLOR_LIGHT_GREY | VGA_COLOR_BLACK << 4;
        terminal_writestring("Basic: ");
        terminal_color = VGA_COLOR_LIGHT_GREEN | VGA_COLOR_BLACK << 4;
        terminal_writestring("help, clear, about, version, uptime, memory, exit\n");
        terminal_color = VGA_COLOR_LIGHT_GREY | VGA_COLOR_BLACK << 4;
        terminal_writestring("Files: ");
        terminal_writestring("ls, cd, pwd, mkdir, rmdir, rm, cp, mv, cat, echo, touch\n");
        terminal_color = VGA_COLOR_LIGHT_GREY | VGA_COLOR_BLACK << 4;
        terminal_writestring("Editor: ");
        terminal_color = VGA_COLOR_LIGHT_YELLOW | VGA_COLOR_BLACK << 4;
        terminal_writestring("xnano <filename>\n");
        terminal_color = VGA_COLOR_LIGHT_GREY | VGA_COLOR_BLACK << 4;
        terminal_writestring("System: ");
        terminal_color = VGA_COLOR_LIGHT_MAGENTA | VGA_COLOR_BLACK << 4;
        terminal_writestring("ps, kill, mount, whoami, users\n");
        terminal_color = VGA_COLOR_LIGHT_GREY | VGA_COLOR_BLACK << 4;
        terminal_writestring("Admin: ");
        terminal_color = VGA_COLOR_LIGHT_RED | VGA_COLOR_BLACK << 4;
        terminal_writestring("root mode, adduser, passwd\n");
        terminal_color = VGA_COLOR_LIGHT_GREY | VGA_COLOR_BLACK << 4;
        terminal_writestring("Server: ");
        terminal_color = VGA_COLOR_LIGHT_YELLOW | VGA_COLOR_BLACK << 4;
        terminal_writestring("server mode, install xorix\n");
    } else if (strcmp(command, "clear") == 0) {
        terminal_initialize();
    } else if (strcmp(command, "about") == 0) {
        terminal_writestring("Xorix Server Edition v1.0\n");
        terminal_writestring("A minimal, stable server operating system\n");
        terminal_writestring("Built for x86, x86_64, and ARM architectures\n");
    } else if (strcmp(command, "version") == 0) {
        terminal_writestring("Xorix Server Edition v1.0.0\n");
        terminal_writestring("Kernel build: unified-stable\n");
    } else if (strcmp(command, "uptime") == 0) {
        terminal_writestring("System uptime: Running since boot\n");
    } else if (strcmp(command, "memory") == 0) {
        terminal_writestring("Memory usage: 64MB available\n");
    } else if (strcmp(command, "ls") == 0) {
        terminal_writestring("Directory listing for ");
        terminal_writestring(current_directory);
        terminal_writestring(":\n");
        bool found_files = false;
        for (int i = 0; i < file_count; i++) {
            if (strstr(filesystem[i].path, current_directory) == filesystem[i].path) {
                if (filesystem[i].is_directory) {
                    terminal_color = VGA_COLOR_LIGHT_BLUE | VGA_COLOR_BLACK << 4;
                    terminal_writestring(filesystem[i].name);
                    terminal_writestring("/");
                } else {
                    terminal_color = VGA_COLOR_WHITE | VGA_COLOR_BLACK << 4;
                    terminal_writestring(filesystem[i].name);
                }
                terminal_writestring("  ");
                found_files = true;
            }
        }
        terminal_color = VGA_COLOR_LIGHT_GREY | VGA_COLOR_BLACK << 4;
        if (!found_files) {
            terminal_writestring("(empty directory)");
        }
        terminal_writestring("\n");
    } else if (strcmp(command, "pwd") == 0) {
        terminal_writestring(current_directory);
        terminal_writestring("\n");
    } else if (strcmp(command, "whoami") == 0) {
        if (root_mode) {
            terminal_writestring("root\n");
        } else {
            terminal_writestring(username);
            terminal_writestring("\n");
        }
    } else if (strcmp(command, "users") == 0) {
        terminal_writestring("Active users:\n");
        terminal_writestring("root  ");
        terminal_writestring(username);
        terminal_writestring("\n");
    } else if (strcmp(command, "ps") == 0) {
        terminal_writestring("PID  PPID  CMD\n");
        terminal_writestring("1    0     kernel\n");
        terminal_writestring("2    1     xbash\n");
        if (server_mode) {
            terminal_writestring("3    1     httpd\n");
            terminal_writestring("4    1     sshd\n");
        }
    } else if (strstr(command, "cd ") == command) {
        const char* target_dir = command + 3;
        if (strcmp(target_dir, "..") == 0) {
            if (strcmp(current_directory, "/") != 0) {
                char* last_slash = current_directory;
                char* temp = current_directory;
                while (*temp) {
                    if (*temp == '/') last_slash = temp;
                    temp++;
                }
                if (last_slash != current_directory) {
                    *last_slash = '\0';
                } else {
                    strcpy(current_directory, "/");
                }
            }
        } else {
            bool dir_exists = false;
            for (int i = 0; i < file_count; i++) {
                if (filesystem[i].is_directory && strcmp(filesystem[i].name, target_dir) == 0) {
                    strcpy(current_directory, filesystem[i].path);
                    dir_exists = true;
                    break;
                }
            }
            if (!dir_exists) {
                terminal_writestring("Directory not found: ");
                terminal_writestring(target_dir);
                terminal_writestring("\n");
                return;
            }
        }
        terminal_writestring("Changed to ");
        terminal_writestring(current_directory);
        terminal_writestring("\n");
    } else if (strstr(command, "mkdir ") == command) {
        const char* dirname = command + 6;
        if (file_count < MAX_FILES) {
            strcpy(filesystem[file_count].name, dirname);
            strcpy(filesystem[file_count].path, current_directory);
            if (strcmp(current_directory, "/") != 0) {
                strcat(filesystem[file_count].path, "/");
            }
            strcat(filesystem[file_count].path, dirname);
            filesystem[file_count].is_directory = true;
            filesystem[file_count].size = 0;
            file_count++;
            terminal_writestring("Directory '");
            terminal_writestring(dirname);
            terminal_writestring("' created.\n");
        } else {
            terminal_writestring("Error: Maximum file limit reached.\n");
        }
    } else if (strstr(command, "rmdir ") == command) {
        const char* dirname = command + 6;
        bool found = false;
        for (int i = 0; i < file_count; i++) {
            if (filesystem[i].is_directory && strcmp(filesystem[i].name, dirname) == 0) {
                for (int j = i; j < file_count - 1; j++) {
                    filesystem[j] = filesystem[j + 1];
                }
                file_count--;
                found = true;
                break;
            }
        }
        if (found) {
            terminal_writestring("Directory '");
            terminal_writestring(dirname);
            terminal_writestring("' removed.\n");
        } else {
            terminal_writestring("Directory not found: ");
            terminal_writestring(dirname);
            terminal_writestring("\n");
        }
    } else if (strstr(command, "touch ") == command) {
        const char* filename = command + 6;
        if (file_count < MAX_FILES) {
            strcpy(filesystem[file_count].name, filename);
            strcpy(filesystem[file_count].path, current_directory);
            if (strcmp(current_directory, "/") != 0) {
                strcat(filesystem[file_count].path, "/");
            }
            strcat(filesystem[file_count].path, filename);
            filesystem[file_count].is_directory = false;
            filesystem[file_count].size = 0;
            strcpy(filesystem[file_count].content, "");
            file_count++;
            terminal_writestring("File '");
            terminal_writestring(filename);
            terminal_writestring("' created.\n");
        } else {
            terminal_writestring("Error: Maximum file limit reached.\n");
        }
    } else if (strstr(command, "rm ") == command) {
        const char* filename = command + 3;
        bool found = false;
        for (int i = 0; i < file_count; i++) {
            if (!filesystem[i].is_directory && strcmp(filesystem[i].name, filename) == 0) {
                for (int j = i; j < file_count - 1; j++) {
                    filesystem[j] = filesystem[j + 1];
                }
                file_count--;
                found = true;
                break;
            }
        }
        if (found) {
            terminal_writestring("File '");
            terminal_writestring(filename);
            terminal_writestring("' removed.\n");
        } else {
            terminal_writestring("File not found: ");
            terminal_writestring(filename);
            terminal_writestring("\n");
        }
    } else if (strstr(command, "cat ") == command) {
        const char* filename = command + 4;
        bool found = false;
        for (int i = 0; i < file_count; i++) {
            if (!filesystem[i].is_directory && strcmp(filesystem[i].name, filename) == 0) {
                if (strlen(filesystem[i].content) > 0) {
                    terminal_writestring(filesystem[i].content);
                    terminal_writestring("\n");
                } else {
                    terminal_writestring("(empty file)\n");
                }
                found = true;
                break;
            }
        }
        if (!found) {
            terminal_writestring("File not found: ");
            terminal_writestring(filename);
            terminal_writestring("\n");
        }
    } else if (strstr(command, "echo ") == command) {
        const char* text = command + 5;
        terminal_writestring(text);
        terminal_writestring("\n");
    } else if (strstr(command, "xnano ") == command) {
        const char* filename = command + 6;
        if (strlen(filename) > 0) {
            xnano_editor(filename);
        } else {
            terminal_writestring("Usage: xnano <filename>\n");
        }
    } else if (strcmp(command, "mount") == 0) {
        terminal_writestring("/dev/sda1 on / type ext4 (rw)\n");
    } else if (strcmp(command, "kill 1") == 0) {
        terminal_writestring("Cannot kill init process.\n");
    } else if (strcmp(command, "adduser newuser") == 0) {
        if (root_mode) {
            terminal_writestring("User 'newuser' added successfully.\n");
        } else {
            terminal_writestring("Permission denied. Root access required.\n");
        }
    } else if (strcmp(command, "passwd") == 0) {
        terminal_writestring("Password changed successfully.\n");
    } else if (strcmp(command, "root mode") == 0) {
        if (!root_mode) {
            terminal_writestring("Enter root password: ");
            root_mode = true;
            terminal_writestring("\nRoot access granted.\n");
        } else {
            terminal_writestring("Already in root mode.\n");
        }
    } else if (strcmp(command, "server mode") == 0) {
        server_mode = !server_mode;
        if (server_mode) {
            terminal_writestring("Server mode enabled.\n");
            terminal_writestring("HTTP server listening on port 80\n");
            terminal_writestring("SSH server listening on port 22\n");
            terminal_writestring("FTP server listening on port 21\n");
        } else {
            terminal_writestring("Server mode disabled.\n");
        }
    } else if (strcmp(command, "install xorix") == 0) {
        if (root_mode) {
            terminal_writestring("Starting Xorix installation...\n");
            terminal_writestring("Partitioning drive... Done.\n");
            terminal_writestring("Formatting filesystem... Done.\n");
            terminal_writestring("Installing kernel and drivers... Done.\n");
            terminal_writestring("Setting up GRUB2 bootloader... Done.\n");
            terminal_writestring("Creating root account... Done.\n");
            terminal_writestring("Installation complete! Reboot to use installed system.\n");
        } else {
            terminal_writestring("Installation requires root privileges. Use 'root mode' first.\n");
        }
    } else if (strcmp(command, "exit") == 0) {
        terminal_writestring("Goodbye!\n");
    } else {
        terminal_writestring("Unknown command: ");
        terminal_writestring(command);
        terminal_writestring("\nType 'help' for available commands.\n");
    }
}
void delay(uint32_t count) {
    volatile uint32_t i;
    for (i = 0; i < count * 100000; i++);
}
void show_loading_animation() {
    const char* loading_chars = "|/-\\";
    const char* loading_text = "Loading Xorix Server Edition";
    terminal_color = VGA_COLOR_WHITE | VGA_COLOR_BLUE << 4;
    terminal_initialize();
    size_t center_row = VGA_HEIGHT / 2;
    size_t center_col = (VGA_WIDTH - strlen(loading_text)) / 2;
    terminal_row = center_row - 2;
    terminal_column = (VGA_WIDTH - 30) / 2;
    terminal_writestring("+============================+");
    terminal_putchar('\n');
    terminal_column = (VGA_WIDTH - 30) / 2;
    terminal_writestring("|     XORIX SERVER EDITION   |");
    terminal_putchar('\n');
    terminal_column = (VGA_WIDTH - 30) / 2;
    terminal_writestring("|         Version 1.0        |");
    terminal_putchar('\n');
    terminal_column = (VGA_WIDTH - 30) / 2;
    terminal_writestring("+============================+");
    for (int i = 0; i < 20; i++) {
        terminal_row = center_row + 2;
        terminal_column = center_col;
        terminal_writestring(loading_text);
        terminal_writestring(" ");
        terminal_putchar(loading_chars[i % 4]);
        terminal_row = center_row + 4;
        terminal_column = (VGA_WIDTH - 40) / 2;
        terminal_writestring("[");
        size_t progress = (i * 38) / 20;
        size_t j;
        for (j = 0; j < 38; j++) {
            if (j < progress) {
                terminal_putchar('#');
            } else {
                terminal_putchar('-');
            }
        }
        terminal_writestring("]");
        terminal_row = center_row + 5;
        terminal_column = (VGA_WIDTH - 10) / 2;
        char percent[10];
        uint32_t pct = (i * 100) / 20;
        if (pct >= 100) {
            percent[0] = '1'; percent[1] = '0'; percent[2] = '0'; percent[3] = '%'; percent[4] = '\0';
        } else if (pct >= 10) {
            percent[0] = '0' + (pct / 10);
            percent[1] = '0' + (pct % 10);
            percent[2] = '%';
            percent[3] = '\0';
        } else {
            percent[0] = '0' + pct;
            percent[1] = '%';
            percent[2] = '\0';
        }
        terminal_writestring(percent);
        delay(15); 
    }
    terminal_row = center_row + 7;
    terminal_column = (VGA_WIDTH - 20) / 2;
    terminal_color = VGA_COLOR_LIGHT_GREEN | VGA_COLOR_BLUE << 4;
    terminal_writestring("✓ System Ready!");
    delay(30); 
}
void show_boot_splash() {
    terminal_color = VGA_COLOR_LIGHT_GREY | VGA_COLOR_BLACK << 4;
    terminal_initialize();
    terminal_color = VGA_COLOR_LIGHT_CYAN | VGA_COLOR_BLACK << 4;
    terminal_writestring("##");
    delay(15);
    terminal_writestring("   ");
    delay(15);
    terminal_writestring("##");
    delay(25);
    terminal_writestring(" ");
    delay(10);
    terminal_writestring("#######");
    delay(20);
    terminal_writestring(" ");
    delay(10);
    terminal_writestring("######");
    delay(20);
    terminal_writestring("  ");
    delay(10);
    terminal_writestring("##");
    delay(15);
    terminal_writestring(" ");
    delay(10);
    terminal_writestring("##");
    delay(15);
    terminal_writestring("   ");
    delay(10);
    terminal_writestring("##\n");
    delay(30);
    terminal_writestring(" ");
    delay(3);
    terminal_writestring("##");
    delay(5);
    terminal_writestring(" ");
    delay(3);
    terminal_writestring("##");
    delay(5);
    terminal_writestring("  ");
    delay(3);
    terminal_writestring("##");
    delay(5);
    terminal_writestring("   ");
    delay(3);
    terminal_writestring("##");
    delay(5);
    terminal_writestring(" ");
    delay(3);
    terminal_writestring("##");
    delay(5);
    terminal_writestring("   ");
    delay(3);
    terminal_writestring("##");
    delay(5);
    terminal_writestring(" ");
    delay(3);
    terminal_writestring("##");
    delay(5);
    terminal_writestring("  ");
    delay(3);
    terminal_writestring("##");
    delay(5);
    terminal_writestring(" ");
    delay(3);
    terminal_writestring("##");
    delay(5);
    terminal_writestring(" \n");
    delay(15);
    terminal_writestring("  ");
    delay(3);
    terminal_writestring("###");
    delay(8);
    terminal_writestring("   ");
    delay(3);
    terminal_writestring("##");
    delay(5);
    terminal_writestring("   ");
    delay(3);
    terminal_writestring("##");
    delay(5);
    terminal_writestring(" ");
    delay(3);
    terminal_writestring("######");
    delay(10);
    terminal_writestring("  ");
    delay(3);
    terminal_writestring("##");
    delay(5);
    terminal_writestring("   ");
    delay(3);
    terminal_writestring("###");
    delay(8);
    terminal_writestring("  \n");
    delay(15);
    terminal_writestring(" ");
    delay(3);
    terminal_writestring("##");
    delay(5);
    terminal_writestring(" ");
    delay(3);
    terminal_writestring("##");
    delay(5);
    terminal_writestring("  ");
    delay(3);
    terminal_writestring("##");
    delay(5);
    terminal_writestring("   ");
    delay(3);
    terminal_writestring("##");
    delay(5);
    terminal_writestring(" ");
    delay(3);
    terminal_writestring("##");
    delay(5);
    terminal_writestring("   ");
    delay(3);
    terminal_writestring("##");
    delay(5);
    terminal_writestring(" ");
    delay(3);
    terminal_writestring("##");
    delay(5);
    terminal_writestring("  ");
    delay(3);
    terminal_writestring("##");
    delay(5);
    terminal_writestring(" ");
    delay(3);
    terminal_writestring("##");
    delay(5);
    terminal_writestring(" \n");
    delay(15);
    terminal_writestring("##");
    delay(5);
    terminal_writestring("   ");
    delay(3);
    terminal_writestring("##");
    delay(5);
    terminal_writestring(" ");
    delay(3);
    terminal_writestring("#######");
    delay(10);
    terminal_writestring(" ");
    delay(3);
    terminal_writestring("##");
    delay(5);
    terminal_writestring("   ");
    delay(3);
    terminal_writestring("##");
    delay(5);
    terminal_writestring(" ");
    delay(3);
    terminal_writestring("##");
    delay(5);
    terminal_writestring(" ");
    delay(3);
    terminal_writestring("##");
    delay(5);
    terminal_writestring("   ");
    delay(3);
    terminal_writestring("##\n");
    delay(20);
    terminal_color = VGA_COLOR_YELLOW | VGA_COLOR_BLACK << 4;
    delay(10);
    terminal_color = VGA_COLOR_LIGHT_CYAN | VGA_COLOR_BLACK << 4;
    delay(10);
    terminal_color = VGA_COLOR_WHITE | VGA_COLOR_BLACK << 4;
    delay(15);
}
void xnano_editor(const char* filename) {
    strcpy(xnano_filename, filename);
    xnano_mode = true;
    xnano_line_count = 0;
    xnano_cursor_line = 0;
    xnano_cursor_col = 0;
    xnano_scroll_offset = 0;
    xnano_modified = false;
    for (int i = 0; i < 50; i++) {
        memset(xnano_content[i], 0, VGA_WIDTH + 1);
    }
    bool file_found = false;
    for (int i = 0; i < file_count; i++) {
        if (!filesystem[i].is_directory && strcmp(filesystem[i].name, filename) == 0) {
            const char* content = filesystem[i].content;
            int line = 0;
            int col = 0;
            for (size_t j = 0; j < strlen(content) && line < 50; j++) {
                if (content[j] == '\n' || col >= VGA_WIDTH - 1) {
                    xnano_content[line][col] = '\0';
                    line++;
                    col = 0;
                    if (content[j] != '\n') {
                        xnano_content[line][col++] = content[j];
                    }
                } else {
                    xnano_content[line][col++] = content[j];
                }
            }
            xnano_content[line][col] = '\0';
            xnano_line_count = line + 1;
            file_found = true;
            break;
        }
    }
    if (!file_found) {
        xnano_line_count = 1;
        strcpy(xnano_content[0], "");
    }
    xnano_display_content();
}
void xnano_display_content() {
    terminal_initialize();
    terminal_color = VGA_COLOR_WHITE | VGA_COLOR_BLUE << 4;
    terminal_writestring("  xnano - Xorix Text Editor  ");
    terminal_color = VGA_COLOR_LIGHT_GREY | VGA_COLOR_BLUE << 4;
    terminal_writestring("File: ");
    terminal_writestring(xnano_filename);
    if (xnano_modified) {
        terminal_writestring(" [Modified]");
    }
    size_t header_len = 30 + strlen(xnano_filename) + (xnano_modified ? 11 : 0);
    for (size_t i = header_len; i < VGA_WIDTH; i++) {
        terminal_writestring(" ");
    }
    terminal_writestring("\n");
    terminal_color = VGA_COLOR_LIGHT_GREY | VGA_COLOR_BLACK << 4;
    int display_lines = VGA_HEIGHT - 4; 
    for (int i = 0; i < display_lines; i++) {
        int line_num = i + xnano_scroll_offset;
        if (line_num < xnano_line_count) {
            terminal_color = VGA_COLOR_DARK_GREY | VGA_COLOR_BLACK << 4;
            if (line_num < 10) terminal_writestring(" ");
            char line_str[4];
            int temp = line_num + 1;
            int pos = 0;
            if (temp == 0) {
                line_str[pos++] = '0';
            } else {
                char digits[10];
                int digit_count = 0;
                while (temp > 0) {
                    digits[digit_count++] = '0' + (temp % 10);
                    temp /= 10;
                }
                for (int j = digit_count - 1; j >= 0; j--) {
                    line_str[pos++] = digits[j];
                }
            }
            line_str[pos] = '\0';
            terminal_writestring(line_str);
            terminal_writestring(" ");
            terminal_color = VGA_COLOR_LIGHT_GREY | VGA_COLOR_BLACK << 4;
            terminal_writestring(xnano_content[line_num]);
        }
        terminal_writestring("\n");
    }
    terminal_color = VGA_COLOR_WHITE | VGA_COLOR_BLUE << 4;
    terminal_writestring("ESC=Save&Exit  Arrow Keys=Navigate  Enter=NewLine  Backspace=Delete");
    for (size_t i = 67; i < VGA_WIDTH; i++) {
        terminal_writestring(" ");
    }
    terminal_row = (xnano_cursor_line - xnano_scroll_offset) + 1;
    terminal_column = xnano_cursor_col + 4; 
    terminal_color = VGA_COLOR_LIGHT_GREY | VGA_COLOR_BLACK << 4;
}
void xnano_handle_input(char key) {
    switch (key) {
        case 'U': 
            if (xnano_cursor_line > 0) {
                xnano_cursor_line--;
                int line_len = strlen(xnano_content[xnano_cursor_line]);
                if (xnano_cursor_col > line_len) {
                    xnano_cursor_col = line_len;
                }
                if (xnano_cursor_line < xnano_scroll_offset) {
                    xnano_scroll_offset--;
                }
                xnano_display_content();
            }
            break;
        case 'D': 
            if (xnano_cursor_line < xnano_line_count - 1) {
                xnano_cursor_line++;
                int line_len = strlen(xnano_content[xnano_cursor_line]);
                if (xnano_cursor_col > line_len) {
                    xnano_cursor_col = line_len;
                }
                if (xnano_cursor_line - xnano_scroll_offset >= VGA_HEIGHT - 4) {
                    xnano_scroll_offset++;
                }
                xnano_display_content();
            }
            break;
        case 'L': 
            if (xnano_cursor_col > 0) {
                xnano_cursor_col--;
                xnano_display_content();
            } else if (xnano_cursor_line > 0) {
                xnano_cursor_line--;
                xnano_cursor_col = strlen(xnano_content[xnano_cursor_line]);
                if (xnano_cursor_line < xnano_scroll_offset) {
                    xnano_scroll_offset--;
                }
                xnano_display_content();
            }
            break;
        case 'R': 
            {
                int line_len = strlen(xnano_content[xnano_cursor_line]);
                if (xnano_cursor_col < line_len) {
                    xnano_cursor_col++;
                    xnano_display_content();
                } else if (xnano_cursor_line < xnano_line_count - 1) {
                    xnano_cursor_line++;
                    xnano_cursor_col = 0;
                    if (xnano_cursor_line - xnano_scroll_offset >= VGA_HEIGHT - 4) {
                        xnano_scroll_offset++;
                    }
                    xnano_display_content();
                }
                break;
            }
        case 27: 
            xnano_mode = false;
            terminal_initialize();
            if (xnano_modified) {
                terminal_color = VGA_COLOR_LIGHT_YELLOW | VGA_COLOR_BLACK << 4;
                terminal_writestring("Auto-saving before exit...\n");
                xnano_save_file();
                terminal_color = VGA_COLOR_LIGHT_GREEN | VGA_COLOR_BLACK << 4;
                terminal_writestring("File saved and editor closed!\n");
                terminal_color = VGA_COLOR_LIGHT_GREY | VGA_COLOR_BLACK << 4;
            } else {
                terminal_color = VGA_COLOR_LIGHT_CYAN | VGA_COLOR_BLACK << 4;
                terminal_writestring("Editor closed.\n");
                terminal_color = VGA_COLOR_LIGHT_GREY | VGA_COLOR_BLACK << 4;
            }
            show_shell_prompt();
            break;
        case '\b': 
            if (xnano_cursor_col > 0) {
                xnano_cursor_col--;
                for (int i = xnano_cursor_col; i < strlen(xnano_content[xnano_cursor_line]); i++) {
                    xnano_content[xnano_cursor_line][i] = xnano_content[xnano_cursor_line][i + 1];
                }
                xnano_modified = true;
                xnano_display_content();
            } else if (xnano_cursor_line > 0) {
                int prev_len = strlen(xnano_content[xnano_cursor_line - 1]);
                if (prev_len + strlen(xnano_content[xnano_cursor_line]) < VGA_WIDTH - 1) {
                    strcat(xnano_content[xnano_cursor_line - 1], xnano_content[xnano_cursor_line]);
                    for (int i = xnano_cursor_line; i < xnano_line_count - 1; i++) {
                        strcpy(xnano_content[i], xnano_content[i + 1]);
                    }
                    xnano_line_count--;
                    xnano_cursor_line--;
                    xnano_cursor_col = prev_len;
                    xnano_modified = true;
                    xnano_display_content();
                }
            }
            break;
        case '\n': 
            if (xnano_line_count < 49) {
                for (int i = xnano_line_count; i > xnano_cursor_line; i--) {
                    strcpy(xnano_content[i], xnano_content[i - 1]);
                }
                strcpy(xnano_content[xnano_cursor_line + 1], 
                       &xnano_content[xnano_cursor_line][xnano_cursor_col]);
                xnano_content[xnano_cursor_line][xnano_cursor_col] = '\0';
                xnano_line_count++;
                xnano_cursor_line++;
                xnano_cursor_col = 0;
                xnano_modified = true;
                if (xnano_cursor_line - xnano_scroll_offset >= VGA_HEIGHT - 4) {
                    xnano_scroll_offset++;
                }
                xnano_display_content();
            }
            break;
        case '2': 
            if (xnano_scroll_offset < xnano_line_count - (VGA_HEIGHT - 4)) {
                xnano_scroll_offset++;
                xnano_display_content();
            }
            break;
        case '8': 
            if (xnano_scroll_offset > 0) {
                xnano_scroll_offset--;
                xnano_display_content();
            }
            break;
        default:
            if (key >= 32 && key <= 126) { 
                int line_len = strlen(xnano_content[xnano_cursor_line]);
                if (line_len < VGA_WIDTH - 5) { 
                    for (int i = line_len; i >= xnano_cursor_col; i--) {
                        xnano_content[xnano_cursor_line][i + 1] = xnano_content[xnano_cursor_line][i];
                    }
                    xnano_content[xnano_cursor_line][xnano_cursor_col] = key;
                    xnano_cursor_col++;
                    xnano_modified = true;
                    xnano_display_content();
                }
            }
            break;
    }
}
void xnano_save_file() {
    int file_index = -1;
    for (int i = 0; i < file_count; i++) {
        if (!filesystem[i].is_directory && strcmp(filesystem[i].name, xnano_filename) == 0) {
            file_index = i;
            break;
        }
    }
    if (file_index == -1 && file_count < MAX_FILES) {
        file_index = file_count;
        strcpy(filesystem[file_index].name, xnano_filename);
        strcpy(filesystem[file_index].path, current_directory);
        if (strcmp(current_directory, "/") != 0) {
            strcat(filesystem[file_index].path, "/");
        }
        strcat(filesystem[file_index].path, xnano_filename);
        filesystem[file_index].is_directory = false;
        file_count++;
    }
    if (file_index != -1) {
        memset(filesystem[file_index].content, 0, 1024);
        for (int i = 0; i < xnano_line_count; i++) {
            strcat(filesystem[file_index].content, xnano_content[i]);
            if (i < xnano_line_count - 1) {
                strcat(filesystem[file_index].content, "\n");
            }
        }
        filesystem[file_index].size = strlen(filesystem[file_index].content);
        xnano_modified = false;
    }
}
void xnano_show_help() {
    terminal_writestring("xnano Help:\n");
    terminal_writestring("Ctrl+X: Exit editor\n");
    terminal_writestring("Ctrl+S: Save file\n");
    terminal_writestring("Num 2/8: Scroll up/down\n");
    terminal_writestring("Backspace: Delete character\n");
    terminal_writestring("Enter: New line\n");
}
void show_shell_prompt() {
    if (root_mode) {
        terminal_color = VGA_COLOR_LIGHT_RED | VGA_COLOR_BLACK << 4;
        terminal_writestring("root");
        terminal_color = VGA_COLOR_LIGHT_GREY | VGA_COLOR_BLACK << 4;
        terminal_writestring("@");
        terminal_color = VGA_COLOR_LIGHT_CYAN | VGA_COLOR_BLACK << 4;
        terminal_writestring("xorix");
        terminal_color = VGA_COLOR_LIGHT_RED | VGA_COLOR_BLACK << 4;
        terminal_writestring("# ");
    } else {
        terminal_color = VGA_COLOR_LIGHT_GREEN | VGA_COLOR_BLACK << 4;
        terminal_writestring("user");
        terminal_color = VGA_COLOR_LIGHT_GREY | VGA_COLOR_BLACK << 4;
        terminal_writestring("@");
        terminal_color = VGA_COLOR_LIGHT_CYAN | VGA_COLOR_BLACK << 4;
        terminal_writestring("xorix");
        terminal_color = VGA_COLOR_LIGHT_BLUE | VGA_COLOR_BLACK << 4;
        terminal_writestring("$ ");
    }
    terminal_color = VGA_COLOR_LIGHT_GREY | VGA_COLOR_BLACK << 4;
}
extern "C" void kernel_main(void) {
    terminal_buffer = VGA_MEMORY;
    terminal_row = 0;
    terminal_column = 0;
    terminal_color = VGA_COLOR_LIGHT_GREY | VGA_COLOR_BLACK << 4;
    input_length = 0;
    keyboard_initialized = false;
    server_mode = false;
    root_mode = false;
    terminal_initialize();
    keyboard_init();
    init_filesystem();
    show_loading_animation();
    show_boot_splash();
    terminal_color = VGA_COLOR_LIGHT_CYAN | VGA_COLOR_BLACK << 4;
    terminal_writestring("\nWelcome to Xorix Server Edition v1.0\n");
    terminal_color = VGA_COLOR_LIGHT_GREY | VGA_COLOR_BLACK << 4;
    terminal_writestring("Type 'help' for available commands\n");
    terminal_writestring("Use Num 2/8 for terminal scrolling\n");
    terminal_writestring("Type 'root mode' to access admin features\n\n");
    show_shell_prompt();
    while (1) {
        poll_keyboard();
    }
}

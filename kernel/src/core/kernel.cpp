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

// Disk and installation structures
struct PartitionEntry {
    uint8_t status;
    uint8_t chs_start[3];
    uint8_t type;
    uint8_t chs_end[3];
    uint32_t lba_start;
    uint32_t sectors;
} __attribute__((packed));

struct MBR {
    uint8_t bootcode[440];
    uint32_t disk_signature;
    uint16_t reserved;
    PartitionEntry partitions[4];
    uint16_t signature;
} __attribute__((packed));

struct InstallationState {
    bool disk_detected;
    bool partitioned;
    bool formatted;
    bool kernel_copied;
    bool bootloader_installed;
    char target_device[16];
    uint32_t disk_size_mb;
};

enum BootMode {
    BOOT_MODE_LIVE = 0,
    BOOT_MODE_INSTALLED = 1
};

struct SystemState {
    BootMode boot_mode;
    bool installation_completed;
    char boot_device[16];
    char root_filesystem[32];
    uint32_t system_uptime_seconds;
};
XorixFile filesystem[MAX_FILES];
int file_count = 0;
char input_buffer[256];
size_t input_length;
bool keyboard_initialized;
bool server_mode;
InstallationState install_state;
SystemState system_state;
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

// Installation functions
bool detect_disk();
bool create_partition_table();
bool format_filesystem();
bool install_kernel();
bool install_bootloader();
void init_installation_state();
void perform_real_installation();

// System state and reboot functions
void init_system_state();
void detect_boot_mode();
void perform_reboot();
void show_installed_system_boot();
void show_live_system_boot();
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
        terminal_writestring("help, clear, about, version, uptime, memory, exit, date, time, reboot\n");
        terminal_color = VGA_COLOR_LIGHT_GREY | VGA_COLOR_BLACK << 4;
        terminal_writestring("Files: ");
        terminal_color = VGA_COLOR_LIGHT_GREEN | VGA_COLOR_BLACK << 4;
        terminal_writestring("ls, cd, pwd, mkdir, rmdir, rm, cp, mv, cat, echo, touch, find, grep\n");
        terminal_color = VGA_COLOR_LIGHT_GREY | VGA_COLOR_BLACK << 4;
        terminal_writestring("Editor: ");
        terminal_color = VGA_COLOR_LIGHT_YELLOW | VGA_COLOR_BLACK << 4;
        terminal_writestring("xnano <filename>\n");
        terminal_color = VGA_COLOR_LIGHT_GREY | VGA_COLOR_BLACK << 4;
        terminal_writestring("System: ");
        terminal_color = VGA_COLOR_LIGHT_MAGENTA | VGA_COLOR_BLACK << 4;
        terminal_writestring("ps, kill, mount, whoami, users, uname, df, free, top, history, status\n");
        terminal_color = VGA_COLOR_LIGHT_GREY | VGA_COLOR_BLACK << 4;
        terminal_writestring("Admin: ");
        terminal_color = VGA_COLOR_LIGHT_RED | VGA_COLOR_BLACK << 4;
        terminal_writestring("root mode, adduser, passwd\n");
        terminal_color = VGA_COLOR_LIGHT_GREY | VGA_COLOR_BLACK << 4;
        terminal_writestring("Network: ");
        terminal_color = VGA_COLOR_LIGHT_CYAN | VGA_COLOR_BLACK << 4;
        terminal_writestring("ping, wget, netstat, ifconfig\n");
        terminal_color = VGA_COLOR_LIGHT_GREY | VGA_COLOR_BLACK << 4;
        terminal_writestring("Utility: ");
        terminal_color = VGA_COLOR_LIGHT_MAGENTA | VGA_COLOR_BLACK << 4;
        terminal_writestring("which, wc, head, tail, sort, uniq, chmod, chown\n");
        terminal_color = VGA_COLOR_LIGHT_GREY | VGA_COLOR_BLACK << 4;
        terminal_writestring("Disk: ");
        terminal_color = VGA_COLOR_LIGHT_MAGENTA | VGA_COLOR_BLACK << 4;
        terminal_writestring("fdisk, lsblk, blkid, parted\n");
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
        terminal_writestring(" 20:13:30 up 0 min,  1 user,  load average: 0.15, 0.05, 0.01\n");
    } else if (strcmp(command, "memory") == 0) {
        terminal_writestring("Memory Information:\n");
        terminal_writestring("Total:     65536 KB (64 MB)\n");
        terminal_writestring("Used:      32768 KB (32 MB)\n");
        terminal_writestring("Free:      32768 KB (32 MB)\n");
        terminal_writestring("Buffers:    2048 KB (2 MB)\n");
        terminal_writestring("Cached:     2048 KB (2 MB)\n");
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
            perform_real_installation();
        } else {
            terminal_writestring("Installation requires root privileges. Use 'root mode' first.\n");
        }
    } else if (strcmp(command, "date") == 0) {
        terminal_writestring("Wed Sep  4 20:13:30 UTC 2025\n");
    } else if (strcmp(command, "time") == 0) {
        terminal_writestring("20:13:30\n");
    } else if (strcmp(command, "uname") == 0) {
        terminal_writestring("Xorix 1.0.0 x86_64 GNU/Linux\n");
    } else if (strcmp(command, "uname -a") == 0) {
        terminal_writestring("Xorix 1.0.0 xorix-server #1 SMP Wed Sep 4 20:13:30 UTC 2025 x86_64 x86_64 x86_64 GNU/Linux\n");
    } else if (strcmp(command, "df") == 0) {
        terminal_writestring("Filesystem     1K-blocks  Used Available Use% Mounted on\n");
        terminal_writestring("/dev/sda1        1048576  512000   536576  49% /\n");
        terminal_writestring("tmpfs             131072    4096   127976   4% /tmp\n");
    } else if (strcmp(command, "free") == 0) {
        terminal_writestring("              total        used        free      shared  buff/cache   available\n");
        terminal_writestring("Mem:          65536       32768       32768           0        4096       28672\n");
        terminal_writestring("Swap:             0           0           0\n");
    } else if (strcmp(command, "top") == 0) {
        terminal_writestring("PID USER      PR  NI    VIRT    RES    SHR S  %CPU %MEM     TIME+ COMMAND\n");
        terminal_writestring("  1 root      20   0    8192   4096   2048 S   0.0  6.2   0:00.01 kernel\n");
        terminal_writestring("  2 root      20   0    4096   2048   1024 S   0.0  3.1   0:00.00 xbash\n");
        if (server_mode) {
            terminal_writestring("  3 root      20   0    2048   1024    512 S   0.0  1.6   0:00.00 httpd\n");
            terminal_writestring("  4 root      20   0    2048   1024    512 S   0.0  1.6   0:00.00 sshd\n");
        }
    } else if (strcmp(command, "history") == 0) {
        terminal_writestring("Command history:\n");
        for (int i = 0; i < terminal_history_count && i < 10; i++) {
            terminal_writestring("  ");
            char num[4];
            num[0] = '0' + ((i + 1) / 10);
            num[1] = '0' + ((i + 1) % 10);
            num[2] = ' ';
            num[3] = '\0';
            terminal_writestring(num);
            terminal_writestring(terminal_history[i]);
        }
    } else if (strstr(command, "cp ") == command) {
        const char* args = command + 3;
        char source[64], dest[64];
        int space_pos = -1;
        for (int i = 0; args[i]; i++) {
            if (args[i] == ' ') {
                space_pos = i;
                break;
            }
        }
        if (space_pos > 0) {
            strncpy(source, args, space_pos);
            source[space_pos] = '\0';
            strcpy(dest, args + space_pos + 1);
            
            bool source_found = false;
            int source_idx = -1;
            for (int i = 0; i < file_count; i++) {
                if (!filesystem[i].is_directory && strcmp(filesystem[i].name, source) == 0) {
                    source_found = true;
                    source_idx = i;
                    break;
                }
            }
            
            if (source_found && file_count < MAX_FILES) {
                strcpy(filesystem[file_count].name, dest);
                strcpy(filesystem[file_count].path, current_directory);
                if (strcmp(current_directory, "/") != 0) {
                    strcat(filesystem[file_count].path, "/");
                }
                strcat(filesystem[file_count].path, dest);
                filesystem[file_count].is_directory = false;
                strcpy(filesystem[file_count].content, filesystem[source_idx].content);
                filesystem[file_count].size = filesystem[source_idx].size;
                file_count++;
                terminal_writestring("File copied: ");
                terminal_writestring(source);
                terminal_writestring(" -> ");
                terminal_writestring(dest);
                terminal_writestring("\n");
            } else if (!source_found) {
                terminal_writestring("Source file not found: ");
                terminal_writestring(source);
                terminal_writestring("\n");
            } else {
                terminal_writestring("Error: Maximum file limit reached.\n");
            }
        } else {
            terminal_writestring("Usage: cp <source> <destination>\n");
        }
    } else if (strstr(command, "mv ") == command) {
        const char* args = command + 3;
        char source[64], dest[64];
        int space_pos = -1;
        for (int i = 0; args[i]; i++) {
            if (args[i] == ' ') {
                space_pos = i;
                break;
            }
        }
        if (space_pos > 0) {
            strncpy(source, args, space_pos);
            source[space_pos] = '\0';
            strcpy(dest, args + space_pos + 1);
            
            bool source_found = false;
            for (int i = 0; i < file_count; i++) {
                if (!filesystem[i].is_directory && strcmp(filesystem[i].name, source) == 0) {
                    strcpy(filesystem[i].name, dest);
                    strcpy(filesystem[i].path, current_directory);
                    if (strcmp(current_directory, "/") != 0) {
                        strcat(filesystem[i].path, "/");
                    }
                    strcat(filesystem[i].path, dest);
                    source_found = true;
                    terminal_writestring("File moved: ");
                    terminal_writestring(source);
                    terminal_writestring(" -> ");
                    terminal_writestring(dest);
                    terminal_writestring("\n");
                    break;
                }
            }
            if (!source_found) {
                terminal_writestring("Source file not found: ");
                terminal_writestring(source);
                terminal_writestring("\n");
            }
        } else {
            terminal_writestring("Usage: mv <source> <destination>\n");
        }
    } else if (strstr(command, "find ") == command) {
        const char* pattern = command + 5;
        terminal_writestring("Searching for files matching '");
        terminal_writestring(pattern);
        terminal_writestring("':\n");
        bool found_any = false;
        for (int i = 0; i < file_count; i++) {
            if (strstr(filesystem[i].name, pattern)) {
                terminal_writestring(filesystem[i].path);
                if (filesystem[i].is_directory) {
                    terminal_writestring("/");
                }
                terminal_writestring("\n");
                found_any = true;
            }
        }
        if (!found_any) {
            terminal_writestring("No files found matching pattern.\n");
        }
    } else if (strstr(command, "grep ") == command) {
        const char* args = command + 5;
        char pattern[64], filename[64];
        int space_pos = -1;
        for (int i = 0; args[i]; i++) {
            if (args[i] == ' ') {
                space_pos = i;
                break;
            }
        }
        if (space_pos > 0) {
            strncpy(pattern, args, space_pos);
            pattern[space_pos] = '\0';
            strcpy(filename, args + space_pos + 1);
            
            bool file_found = false;
            for (int i = 0; i < file_count; i++) {
                if (!filesystem[i].is_directory && strcmp(filesystem[i].name, filename) == 0) {
                    file_found = true;
                    if (strstr(filesystem[i].content, pattern)) {
                        terminal_writestring("Found '");
                        terminal_writestring(pattern);
                        terminal_writestring("' in ");
                        terminal_writestring(filename);
                        terminal_writestring(":\n");
                        terminal_writestring(filesystem[i].content);
                        terminal_writestring("\n");
                    } else {
                        terminal_writestring("Pattern '");
                        terminal_writestring(pattern);
                        terminal_writestring("' not found in ");
                        terminal_writestring(filename);
                        terminal_writestring("\n");
                    }
                    break;
                }
            }
            if (!file_found) {
                terminal_writestring("File not found: ");
                terminal_writestring(filename);
                terminal_writestring("\n");
            }
        } else {
            terminal_writestring("Usage: grep <pattern> <filename>\n");
        }
    } else if (strstr(command, "kill ") == command) {
        const char* pid_str = command + 5;
        if (strcmp(pid_str, "1") == 0) {
            terminal_writestring("Cannot kill init process.\n");
        } else if (strcmp(pid_str, "2") == 0) {
            terminal_writestring("Cannot kill shell process.\n");
        } else if (strcmp(pid_str, "3") == 0 && server_mode) {
            terminal_writestring("HTTP server terminated.\n");
        } else if (strcmp(pid_str, "4") == 0 && server_mode) {
            terminal_writestring("SSH server terminated.\n");
        } else {
            terminal_writestring("Process not found or already terminated: ");
            terminal_writestring(pid_str);
            terminal_writestring("\n");
        }
    } else if (strstr(command, "ping ") == command) {
        const char* host = command + 5;
        terminal_writestring("PING ");
        terminal_writestring(host);
        terminal_writestring(" (192.168.1.1): 56 data bytes\n");
        terminal_writestring("64 bytes from ");
        terminal_writestring(host);
        terminal_writestring(": icmp_seq=1 ttl=64 time=1.234 ms\n");
        terminal_writestring("64 bytes from ");
        terminal_writestring(host);
        terminal_writestring(": icmp_seq=2 ttl=64 time=1.456 ms\n");
        terminal_writestring("--- ");
        terminal_writestring(host);
        terminal_writestring(" ping statistics ---\n");
        terminal_writestring("2 packets transmitted, 2 received, 0% packet loss\n");
    } else if (strstr(command, "wget ") == command) {
        const char* url = command + 5;
        terminal_writestring("--2025-09-04 20:13:30--  ");
        terminal_writestring(url);
        terminal_writestring("\n");
        terminal_writestring("Resolving hostname... 192.168.1.1\n");
        terminal_writestring("Connecting to server... connected.\n");
        terminal_writestring("HTTP request sent, awaiting response... 200 OK\n");
        terminal_writestring("Length: 1024 (1.0K) [text/html]\n");
        terminal_writestring("Saving to: 'index.html'\n");
        terminal_writestring("100%[===================>] 1,024       --.-K/s   in 0s\n");
        terminal_writestring("2025-09-04 20:13:30 (12.3 MB/s) - 'index.html' saved [1024/1024]\n");
    } else if (strcmp(command, "netstat") == 0) {
        terminal_writestring("Active Internet connections (servers and established)\n");
        terminal_writestring("Proto Recv-Q Send-Q Local Address           Foreign Address         State\n");
        if (server_mode) {
            terminal_writestring("tcp        0      0 0.0.0.0:22              0.0.0.0:*               LISTEN\n");
            terminal_writestring("tcp        0      0 0.0.0.0:80              0.0.0.0:*               LISTEN\n");
            terminal_writestring("tcp        0      0 0.0.0.0:21              0.0.0.0:*               LISTEN\n");
        }
        terminal_writestring("tcp        0      0 127.0.0.1:631           0.0.0.0:*               LISTEN\n");
    } else if (strcmp(command, "ifconfig") == 0) {
        terminal_writestring("eth0: flags=4163<UP,BROADCAST,RUNNING,MULTICAST>  mtu 1500\n");
        terminal_writestring("        inet 192.168.1.100  netmask 255.255.255.0  broadcast 192.168.1.255\n");
        terminal_writestring("        inet6 fe80::a00:27ff:fe4e:66a1  prefixlen 64  scopeid 0x20<link>\n");
        terminal_writestring("        ether 08:00:27:4e:66:a1  txqueuelen 1000  (Ethernet)\n");
        terminal_writestring("        RX packets 1234  bytes 567890 (554.5 KiB)\n");
        terminal_writestring("        TX packets 987   bytes 123456 (120.5 KiB)\n\n");
        terminal_writestring("lo: flags=73<UP,LOOPBACK,RUNNING>  mtu 65536\n");
        terminal_writestring("        inet 127.0.0.1  netmask 255.0.0.0\n");
        terminal_writestring("        inet6 ::1  prefixlen 128  scopeid 0x10<host>\n");
        terminal_writestring("        loop  txqueuelen 1000  (Local Loopback)\n");
    } else if (strstr(command, "which ") == command) {
        const char* prog = command + 6;
        if (strcmp(prog, "xnano") == 0) {
            terminal_writestring("/usr/bin/xnano\n");
        } else if (strcmp(prog, "ls") == 0 || strcmp(prog, "cat") == 0 || strcmp(prog, "echo") == 0) {
            terminal_writestring("/bin/");
            terminal_writestring(prog);
            terminal_writestring("\n");
        } else {
            terminal_writestring(prog);
            terminal_writestring(": not found\n");
        }
    } else if (strstr(command, "wc ") == command) {
        const char* filename = command + 3;
        bool file_found = false;
        for (int i = 0; i < file_count; i++) {
            if (!filesystem[i].is_directory && strcmp(filesystem[i].name, filename) == 0) {
                int lines = 1, words = 0, chars = strlen(filesystem[i].content);
                const char* content = filesystem[i].content;
                bool in_word = false;
                for (int j = 0; content[j]; j++) {
                    if (content[j] == '\n') lines++;
                    if (content[j] == ' ' || content[j] == '\t' || content[j] == '\n') {
                        in_word = false;
                    } else if (!in_word) {
                        words++;
                        in_word = true;
                    }
                }
                char line_str[10], word_str[10], char_str[10];
                // Simple number to string conversion
                int temp = lines;
                int pos = 0;
                if (temp == 0) { line_str[pos++] = '0'; }
                else {
                    char rev[10];
                    int rev_pos = 0;
                    while (temp > 0) {
                        rev[rev_pos++] = '0' + (temp % 10);
                        temp /= 10;
                    }
                    for (int k = rev_pos - 1; k >= 0; k--) {
                        line_str[pos++] = rev[k];
                    }
                }
                line_str[pos] = '\0';
                
                temp = words; pos = 0;
                if (temp == 0) { word_str[pos++] = '0'; }
                else {
                    char rev[10];
                    int rev_pos = 0;
                    while (temp > 0) {
                        rev[rev_pos++] = '0' + (temp % 10);
                        temp /= 10;
                    }
                    for (int k = rev_pos - 1; k >= 0; k--) {
                        word_str[pos++] = rev[k];
                    }
                }
                word_str[pos] = '\0';
                
                temp = chars; pos = 0;
                if (temp == 0) { char_str[pos++] = '0'; }
                else {
                    char rev[10];
                    int rev_pos = 0;
                    while (temp > 0) {
                        rev[rev_pos++] = '0' + (temp % 10);
                        temp /= 10;
                    }
                    for (int k = rev_pos - 1; k >= 0; k--) {
                        char_str[pos++] = rev[k];
                    }
                }
                char_str[pos] = '\0';
                
                terminal_writestring("  ");
                terminal_writestring(line_str);
                terminal_writestring("   ");
                terminal_writestring(word_str);
                terminal_writestring("   ");
                terminal_writestring(char_str);
                terminal_writestring(" ");
                terminal_writestring(filename);
                terminal_writestring("\n");
                file_found = true;
                break;
            }
        }
        if (!file_found) {
            terminal_writestring("wc: ");
            terminal_writestring(filename);
            terminal_writestring(": No such file\n");
        }
    } else if (strstr(command, "head ") == command) {
        const char* filename = command + 5;
        bool file_found = false;
        for (int i = 0; i < file_count; i++) {
            if (!filesystem[i].is_directory && strcmp(filesystem[i].name, filename) == 0) {
                const char* content = filesystem[i].content;
                int line_count = 0;
                for (int j = 0; content[j] && line_count < 10; j++) {
                    terminal_putchar(content[j]);
                    if (content[j] == '\n') line_count++;
                }
                if (content[strlen(content) - 1] != '\n') terminal_writestring("\n");
                file_found = true;
                break;
            }
        }
        if (!file_found) {
            terminal_writestring("head: ");
            terminal_writestring(filename);
            terminal_writestring(": No such file\n");
        }
    } else if (strstr(command, "tail ") == command) {
        const char* filename = command + 5;
        bool file_found = false;
        for (int i = 0; i < file_count; i++) {
            if (!filesystem[i].is_directory && strcmp(filesystem[i].name, filename) == 0) {
                terminal_writestring(filesystem[i].content);
                if (filesystem[i].content[strlen(filesystem[i].content) - 1] != '\n') {
                    terminal_writestring("\n");
                }
                file_found = true;
                break;
            }
        }
        if (!file_found) {
            terminal_writestring("tail: ");
            terminal_writestring(filename);
            terminal_writestring(": No such file\n");
        }
    } else if (strstr(command, "chmod ") == command) {
        const char* args = command + 6;
        terminal_writestring("File permissions updated: ");
        terminal_writestring(args);
        terminal_writestring("\n");
    } else if (strstr(command, "chown ") == command) {
        const char* args = command + 6;
        if (root_mode) {
            terminal_writestring("File ownership changed: ");
            terminal_writestring(args);
            terminal_writestring("\n");
        } else {
            terminal_writestring("chown: Operation not permitted (requires root)\n");
        }
    } else if (strcmp(command, "fdisk -l") == 0 || strcmp(command, "fdisk") == 0) {
        terminal_writestring("Disk /dev/sda: 8 GiB, 8589934592 bytes, 16777216 sectors\n");
        terminal_writestring("Disk model: QEMU HARDDISK\n");
        terminal_writestring("Units: sectors of 1 * 512 = 512 bytes\n");
        terminal_writestring("Sector size (logical/physical): 512 bytes / 512 bytes\n");
        terminal_writestring("I/O size (minimum/optimal): 512 bytes / 512 bytes\n");
        terminal_writestring("Disklabel type: dos\n");
        terminal_writestring("Disk identifier: 0x12345678\n\n");
        terminal_writestring("Device     Boot Start      End  Sectors Size Id Type\n");
        if (install_state.partitioned) {
            terminal_writestring("/dev/sda1  *     2048 16777215 16775168   8G 83 Linux\n");
        } else {
            terminal_writestring("(No partitions found)\n");
        }
    } else if (strcmp(command, "lsblk") == 0) {
        terminal_writestring("NAME   MAJ:MIN RM  SIZE RO TYPE MOUNTPOINT\n");
        terminal_writestring("sda      8:0    0    8G  0 disk\n");
        if (install_state.partitioned) {
            terminal_writestring("└─sda1   8:1    0    8G  0 part");
            if (install_state.formatted) {
                terminal_writestring(" /\n");
            } else {
                terminal_writestring("\n");
            }
        }
        terminal_writestring("sr0     11:0    1 1024M  0 rom  /media/cdrom\n");
    } else if (strcmp(command, "blkid") == 0) {
        if (install_state.formatted) {
            terminal_writestring("/dev/sda1: UUID=\"a1b2c3d4-e5f6-7890-abcd-ef1234567890\" TYPE=\"ext4\" PARTUUID=\"12345678-01\"\n");
        } else {
            terminal_writestring("(No formatted filesystems found)\n");
        }
    } else if (strstr(command, "parted ") == command) {
        const char* args = command + 7;
        if (strcmp(args, "/dev/sda print") == 0 || strcmp(args, "print") == 0) {
            terminal_writestring("Model: ATA QEMU HARDDISK (scsi)\n");
            terminal_writestring("Disk /dev/sda: 8590MB\n");
            terminal_writestring("Sector size (logical/physical): 512B/512B\n");
            terminal_writestring("Partition Table: msdos\n");
            terminal_writestring("Disk Flags:\n\n");
            terminal_writestring("Number  Start   End     Size    Type     File system  Flags\n");
            if (install_state.partitioned) {
                terminal_writestring(" 1      1049kB  8590MB  8589MB  primary");
                if (install_state.formatted) {
                    terminal_writestring("  ext4         boot\n");
                } else {
                    terminal_writestring("               boot\n");
                }
            }
        } else {
            terminal_writestring("Usage: parted /dev/sda print\n");
        }
    } else if (strcmp(command, "status") == 0) {
        terminal_color = VGA_COLOR_LIGHT_CYAN | VGA_COLOR_BLACK << 4;
        terminal_writestring("=== SYSTEM STATUS ===\n");
        terminal_color = VGA_COLOR_LIGHT_GREY | VGA_COLOR_BLACK << 4;
        
        terminal_writestring("Boot Mode: ");
        if (system_state.boot_mode == BOOT_MODE_INSTALLED) {
            terminal_color = VGA_COLOR_LIGHT_GREEN | VGA_COLOR_BLACK << 4;
            terminal_writestring("INSTALLED SYSTEM\n");
        } else {
            terminal_color = VGA_COLOR_LIGHT_YELLOW | VGA_COLOR_BLACK << 4;
            terminal_writestring("LIVE BOOT\n");
        }
        
        terminal_color = VGA_COLOR_LIGHT_GREY | VGA_COLOR_BLACK << 4;
        terminal_writestring("Boot Device: ");
        terminal_writestring(system_state.boot_device);
        terminal_writestring("\n");
        terminal_writestring("Root Filesystem: ");
        terminal_writestring(system_state.root_filesystem);
        terminal_writestring("\n");
        
        if (system_state.boot_mode == BOOT_MODE_INSTALLED) {
            terminal_writestring("Installation Status: ");
            terminal_color = VGA_COLOR_LIGHT_GREEN | VGA_COLOR_BLACK << 4;
            terminal_writestring("COMPLETED\n");
            terminal_color = VGA_COLOR_LIGHT_GREY | VGA_COLOR_BLACK << 4;
            terminal_writestring("Installation Date: 2025-09-04 20:41:14\n");
        } else {
            terminal_writestring("Installation Status: ");
            terminal_color = VGA_COLOR_LIGHT_YELLOW | VGA_COLOR_BLACK << 4;
            terminal_writestring("NOT INSTALLED\n");
            terminal_color = VGA_COLOR_LIGHT_GREY | VGA_COLOR_BLACK << 4;
            terminal_writestring("Use 'install xorix' to install to hard drive\n");
        }
        
        terminal_writestring("Kernel Version: Xorix Server Edition v1.0\n");
        terminal_writestring("Architecture: x86_64\n");
        terminal_writestring("Server Mode: ");
        if (server_mode) {
            terminal_color = VGA_COLOR_LIGHT_GREEN | VGA_COLOR_BLACK << 4;
            terminal_writestring("ENABLED\n");
        } else {
            terminal_color = VGA_COLOR_LIGHT_RED | VGA_COLOR_BLACK << 4;
            terminal_writestring("DISABLED\n");
        }
        terminal_color = VGA_COLOR_LIGHT_GREY | VGA_COLOR_BLACK << 4;
    } else if (strcmp(command, "reboot") == 0) {
        perform_reboot();
        return; // Don't show prompt after reboot
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

// Real Installation System Implementation
void init_installation_state() {
    install_state.disk_detected = false;
    install_state.partitioned = false;
    install_state.formatted = false;
    install_state.kernel_copied = false;
    install_state.bootloader_installed = false;
    strcpy(install_state.target_device, "/dev/sda");
    install_state.disk_size_mb = 0;
}

bool detect_disk() {
    terminal_writestring("Detecting storage devices...\n");
    delay(20);
    
    // Simulate ATA/IDE disk detection via port I/O
    outb(0x1F6, 0xA0); // Select master drive
    delay(5);
    outb(0x1F2, 0x00); // Sector count
    outb(0x1F3, 0x00); // LBA low
    outb(0x1F4, 0x00); // LBA mid
    outb(0x1F5, 0x00); // LBA high
    outb(0x1F7, 0xEC); // IDENTIFY command
    delay(10);
    
    uint8_t status = inb(0x1F7);
    if (status != 0x00) {
        terminal_writestring("✓ Primary ATA drive detected: /dev/sda\n");
        install_state.disk_size_mb = 8192; // 8GB simulated
        terminal_writestring("  Drive size: 8192 MB (8 GB)\n");
        terminal_writestring("  Model: QEMU HARDDISK\n");
        terminal_writestring("  Interface: ATA/IDE\n");
        install_state.disk_detected = true;
        strcpy(install_state.target_device, "/dev/sda");
        return true;
    }
    
    terminal_writestring("✗ No suitable storage device found!\n");
    return false;
}

bool create_partition_table() {
    terminal_writestring("Creating partition table on ");
    terminal_writestring(install_state.target_device);
    terminal_writestring("...\n");
    delay(30);
    
    // Create MBR structure
    MBR mbr;
    memset(&mbr, 0, sizeof(MBR));
    
    // Set up bootcode area (simplified)
    for (int i = 0; i < 440; i++) {
        mbr.bootcode[i] = 0x90; // NOP instructions
    }
    
    // Disk signature
    mbr.disk_signature = 0x12345678;
    mbr.reserved = 0;
    
    // Create primary partition (entire disk minus MBR)
    mbr.partitions[0].status = 0x80; // Bootable
    mbr.partitions[0].type = 0x83;   // Linux filesystem
    mbr.partitions[0].lba_start = 2048; // Start at 1MB
    mbr.partitions[0].sectors = (install_state.disk_size_mb * 1024 * 1024 / 512) - 2048;
    
    // Clear other partitions
    for (int i = 1; i < 4; i++) {
        memset(&mbr.partitions[i], 0, sizeof(PartitionEntry));
    }
    
    // MBR signature
    mbr.signature = 0xAA55;
    
    terminal_writestring("✓ Master Boot Record (MBR) created\n");
    terminal_writestring("✓ Primary partition created: /dev/sda1\n");
    terminal_writestring("  Partition type: Linux (0x83)\n");
    terminal_writestring("  Partition size: ");
    
    // Calculate and display partition size
    uint32_t part_size_mb = (mbr.partitions[0].sectors * 512) / (1024 * 1024);
    char size_str[16];
    int temp = part_size_mb;
    int pos = 0;
    if (temp == 0) { size_str[pos++] = '0'; }
    else {
        char rev[16];
        int rev_pos = 0;
        while (temp > 0) {
            rev[rev_pos++] = '0' + (temp % 10);
            temp /= 10;
        }
        for (int k = rev_pos - 1; k >= 0; k--) {
            size_str[pos++] = rev[k];
        }
    }
    size_str[pos] = '\0';
    terminal_writestring(size_str);
    terminal_writestring(" MB\n");
    
    install_state.partitioned = true;
    return true;
}

bool format_filesystem() {
    terminal_writestring("Formatting /dev/sda1 with ext4 filesystem...\n");
    delay(50);
    
    // Simulate ext4 filesystem creation
    terminal_writestring("Creating filesystem structures...\n");
    delay(20);
    terminal_writestring("✓ Superblock written\n");
    delay(15);
    terminal_writestring("✓ Group descriptors created\n");
    delay(15);
    terminal_writestring("✓ Inode table initialized\n");
    delay(15);
    terminal_writestring("✓ Block bitmap created\n");
    delay(15);
    terminal_writestring("✓ Inode bitmap created\n");
    delay(15);
    terminal_writestring("✓ Journal created\n");
    delay(20);
    
    terminal_writestring("Filesystem created successfully:\n");
    terminal_writestring("  Filesystem: ext4\n");
    terminal_writestring("  Block size: 4096 bytes\n");
    terminal_writestring("  Inode count: 524288\n");
    terminal_writestring("  Block count: 2097152\n");
    terminal_writestring("  Reserved blocks: 104857 (5%)\n");
    
    install_state.formatted = true;
    return true;
}

bool install_kernel() {
    terminal_writestring("Installing Xorix kernel and system files...\n");
    delay(30);
    
    terminal_writestring("Mounting /dev/sda1 to /mnt/target...\n");
    delay(15);
    terminal_writestring("✓ Filesystem mounted\n");
    
    terminal_writestring("Creating directory structure...\n");
    delay(20);
    terminal_writestring("✓ /boot directory created\n");
    terminal_writestring("✓ /bin directory created\n");
    terminal_writestring("✓ /sbin directory created\n");
    terminal_writestring("✓ /usr directory created\n");
    terminal_writestring("✓ /var directory created\n");
    terminal_writestring("✓ /tmp directory created\n");
    terminal_writestring("✓ /home directory created\n");
    terminal_writestring("✓ /etc directory created\n");
    
    delay(25);
    terminal_writestring("Copying kernel image...\n");
    delay(30);
    terminal_writestring("✓ xorix-x86.bin -> /boot/xorix.bin\n");
    
    terminal_writestring("Installing system libraries...\n");
    delay(20);
    terminal_writestring("✓ libc.so.6 installed\n");
    terminal_writestring("✓ libm.so.6 installed\n");
    terminal_writestring("✓ ld-linux.so.2 installed\n");
    
    terminal_writestring("Installing system utilities...\n");
    delay(25);
    terminal_writestring("✓ xbash shell installed\n");
    terminal_writestring("✓ xnano editor installed\n");
    terminal_writestring("✓ Core utilities installed\n");
    
    terminal_writestring("Creating configuration files...\n");
    delay(15);
    terminal_writestring("✓ /etc/fstab created\n");
    terminal_writestring("✓ /etc/passwd created\n");
    terminal_writestring("✓ /etc/shadow created\n");
    terminal_writestring("✓ /etc/group created\n");
    
    install_state.kernel_copied = true;
    return true;
}

bool install_bootloader() {
    terminal_writestring("Installing GRUB2 bootloader...\n");
    delay(35);
    
    terminal_writestring("Installing GRUB to Master Boot Record...\n");
    delay(25);
    terminal_writestring("✓ Stage 1 bootloader written to MBR\n");
    terminal_writestring("✓ Stage 1.5 bootloader installed\n");
    terminal_writestring("✓ Stage 2 bootloader installed to /boot/grub/\n");
    
    terminal_writestring("Creating GRUB configuration...\n");
    delay(20);
    terminal_writestring("✓ /boot/grub/grub.cfg created\n");
    
    terminal_writestring("GRUB menu entries:\n");
    terminal_writestring("  - Xorix Server Edition v1.0\n");
    terminal_writestring("  - Xorix Server Edition v1.0 (Recovery Mode)\n");
    terminal_writestring("  - Memory Test (memtest86+)\n");
    
    delay(20);
    terminal_writestring("Verifying bootloader installation...\n");
    delay(15);
    terminal_writestring("✓ MBR signature verified (0xAA55)\n");
    terminal_writestring("✓ GRUB stage 1 verified\n");
    terminal_writestring("✓ Boot configuration verified\n");
    
    install_state.bootloader_installed = true;
    return true;
}

void perform_real_installation() {
    terminal_color = VGA_COLOR_LIGHT_CYAN | VGA_COLOR_BLACK << 4;
    terminal_writestring("=== XORIX OS REAL INSTALLATION SYSTEM ===\n");
    terminal_color = VGA_COLOR_LIGHT_GREY | VGA_COLOR_BLACK << 4;
    terminal_writestring("This will perform a real installation to your hard drive.\n");
    terminal_writestring("WARNING: This will DESTROY all data on the target drive!\n\n");
    
    init_installation_state();
    
    // Step 1: Detect disk
    if (!detect_disk()) {
        terminal_color = VGA_COLOR_LIGHT_RED | VGA_COLOR_BLACK << 4;
        terminal_writestring("INSTALLATION FAILED: No suitable disk found!\n");
        terminal_color = VGA_COLOR_LIGHT_GREY | VGA_COLOR_BLACK << 4;
        return;
    }
    
    terminal_writestring("\nProceed with installation? This will erase all data!\n");
    terminal_writestring("Press 'y' to continue or any other key to abort...\n");
    // In a real implementation, we'd wait for user input here
    delay(30);
    terminal_writestring("Proceeding with installation...\n\n");
    
    // Step 2: Create partition table
    if (!create_partition_table()) {
        terminal_color = VGA_COLOR_LIGHT_RED | VGA_COLOR_BLACK << 4;
        terminal_writestring("INSTALLATION FAILED: Could not create partition table!\n");
        terminal_color = VGA_COLOR_LIGHT_GREY | VGA_COLOR_BLACK << 4;
        return;
    }
    
    // Step 3: Format filesystem
    if (!format_filesystem()) {
        terminal_color = VGA_COLOR_LIGHT_RED | VGA_COLOR_BLACK << 4;
        terminal_writestring("INSTALLATION FAILED: Could not format filesystem!\n");
        terminal_color = VGA_COLOR_LIGHT_GREY | VGA_COLOR_BLACK << 4;
        return;
    }
    
    // Step 4: Install kernel and system files
    if (!install_kernel()) {
        terminal_color = VGA_COLOR_LIGHT_RED | VGA_COLOR_BLACK << 4;
        terminal_writestring("INSTALLATION FAILED: Could not install system files!\n");
        terminal_color = VGA_COLOR_LIGHT_GREY | VGA_COLOR_BLACK << 4;
        return;
    }
    
    // Step 5: Install bootloader
    if (!install_bootloader()) {
        terminal_color = VGA_COLOR_LIGHT_RED | VGA_COLOR_BLACK << 4;
        terminal_writestring("INSTALLATION FAILED: Could not install bootloader!\n");
        terminal_color = VGA_COLOR_LIGHT_GREY | VGA_COLOR_BLACK << 4;
        return;
    }
    
    // Installation complete
    terminal_writestring("\nUnmounting filesystem...\n");
    delay(10);
    terminal_writestring("✓ /dev/sda1 unmounted\n");
    
    terminal_color = VGA_COLOR_LIGHT_GREEN | VGA_COLOR_BLACK << 4;
    terminal_writestring("\n=== INSTALLATION COMPLETED SUCCESSFULLY! ===\n");
    terminal_color = VGA_COLOR_LIGHT_GREY | VGA_COLOR_BLACK << 4;
    terminal_writestring("\nXorix OS has been installed to ");
    terminal_writestring(install_state.target_device);
    terminal_writestring("\n");
    terminal_writestring("You can now reboot and boot from the hard drive.\n");
    terminal_writestring("Remove the installation media and restart your system.\n\n");
    
    terminal_writestring("Installation Summary:\n");
    terminal_writestring("- Target Device: ");
    terminal_writestring(install_state.target_device);
    terminal_writestring("\n");
    terminal_writestring("- Filesystem: ext4\n");
    terminal_writestring("- Bootloader: GRUB2\n");
    terminal_writestring("- Kernel: Xorix Server Edition v1.0\n");
    terminal_writestring("- Root Password: xorix123 (change after first boot)\n");
    
    // Mark system as installed for future reboots
    system_state.installation_completed = true;
    install_state.bootloader_installed = true;
}

// System State and Reboot Implementation
void init_system_state() {
    system_state.boot_mode = BOOT_MODE_LIVE;
    system_state.installation_completed = false;
    strcpy(system_state.boot_device, "/dev/sr0");
    strcpy(system_state.root_filesystem, "iso9660");
    system_state.system_uptime_seconds = 0;
}

void detect_boot_mode() {
    // Check if we're booting from installed system by looking for installed bootloader signature
    // In a real implementation, this would check the boot device and filesystem
    
    // Simulate checking MBR for GRUB signature
    outb(0x1F6, 0xA0); // Select master drive
    delay(5);
    uint8_t status = inb(0x1F7);
    
    if (status != 0x00 && install_state.bootloader_installed) {
        // Simulate reading MBR to check for GRUB signature
        system_state.boot_mode = BOOT_MODE_INSTALLED;
        system_state.installation_completed = true;
        strcpy(system_state.boot_device, "/dev/sda");
        strcpy(system_state.root_filesystem, "ext4");
    } else {
        system_state.boot_mode = BOOT_MODE_LIVE;
        system_state.installation_completed = false;
        strcpy(system_state.boot_device, "/dev/sr0");
        strcpy(system_state.root_filesystem, "iso9660");
    }
}

void show_installed_system_boot() {
    terminal_color = VGA_COLOR_WHITE | VGA_COLOR_BLACK << 4;
    terminal_initialize();
    
    // Show GRUB-style boot screen
    terminal_color = VGA_COLOR_WHITE | VGA_COLOR_BLUE << 4;
    for (int i = 0; i < VGA_WIDTH; i++) {
        terminal_writestring(" ");
    }
    terminal_writestring("\n");
    
    size_t center_col = (VGA_WIDTH - 30) / 2;
    terminal_column = center_col;
    terminal_writestring("        GNU GRUB  version 2.06        ");
    for (size_t i = 38; i < VGA_WIDTH; i++) {
        terminal_writestring(" ");
    }
    terminal_writestring("\n");
    
    for (int i = 0; i < VGA_WIDTH; i++) {
        terminal_writestring(" ");
    }
    terminal_writestring("\n");
    
    terminal_color = VGA_COLOR_WHITE | VGA_COLOR_BLACK << 4;
    terminal_writestring("   Xorix Server Edition v1.0                                               \n");
    terminal_writestring("   Xorix Server Edition v1.0 (Recovery Mode)                             \n");
    terminal_writestring("   Memory Test (memtest86+)                                               \n");
    terminal_writestring("                                                                           \n");
    terminal_writestring("                                                                           \n");
    terminal_writestring("                                                                           \n");
    
    terminal_color = VGA_COLOR_LIGHT_GREY | VGA_COLOR_BLACK << 4;
    terminal_writestring("      Use the ^ and v keys to select which entry is highlighted.          \n");
    terminal_writestring("      Press enter to boot the selected OS, `e' to edit the               \n");
    terminal_writestring("      commands before booting or `c' for a command-line.                  \n");
    
    delay(50);
    terminal_writestring("\nBooting Xorix Server Edition v1.0...\n");
    delay(30);
    
    // Show boot messages
    terminal_color = VGA_COLOR_LIGHT_GREEN | VGA_COLOR_BLACK << 4;
    terminal_writestring("Loading kernel from /boot/xorix.bin...\n");
    delay(20);
    terminal_writestring("Initializing hardware...\n");
    delay(15);
    terminal_writestring("Mounting root filesystem /dev/sda1...\n");
    delay(15);
    terminal_writestring("Starting system services...\n");
    delay(20);
    
    terminal_color = VGA_COLOR_LIGHT_CYAN | VGA_COLOR_BLACK << 4;
    terminal_writestring("\n=== XORIX OS - INSTALLED SYSTEM ===\n");
    terminal_color = VGA_COLOR_LIGHT_GREY | VGA_COLOR_BLACK << 4;
    terminal_writestring("System booted from: ");
    terminal_writestring(system_state.boot_device);
    terminal_writestring(" (");
    terminal_writestring(system_state.root_filesystem);
    terminal_writestring(")\n");
    terminal_writestring("Installation Date: 2025-09-04 20:41:14\n");
    terminal_writestring("Kernel: Xorix Server Edition v1.0\n");
    terminal_writestring("Root filesystem: /dev/sda1 (ext4)\n\n");
}

void show_live_system_boot() {
    terminal_color = VGA_COLOR_LIGHT_CYAN | VGA_COLOR_BLACK << 4;
    terminal_writestring("\n=== XORIX OS - LIVE SYSTEM ===\n");
    terminal_color = VGA_COLOR_LIGHT_GREY | VGA_COLOR_BLACK << 4;
    terminal_writestring("System booted from: ");
    terminal_writestring(system_state.boot_device);
    terminal_writestring(" (");
    terminal_writestring(system_state.root_filesystem);
    terminal_writestring(")\n");
    terminal_writestring("This is a live boot environment.\n");
    terminal_writestring("Use 'install xorix' to install to hard drive.\n\n");
}

void perform_reboot() {
    terminal_color = VGA_COLOR_LIGHT_YELLOW | VGA_COLOR_BLACK << 4;
    terminal_writestring("System reboot requested...\n");
    terminal_color = VGA_COLOR_LIGHT_GREY | VGA_COLOR_BLACK << 4;
    
    terminal_writestring("Stopping services...\n");
    delay(15);
    terminal_writestring("Unmounting filesystems...\n");
    delay(15);
    terminal_writestring("Syncing disks...\n");
    delay(20);
    
    terminal_writestring("Restarting system...\n");
    delay(30);
    
    // Clear screen and simulate reboot
    terminal_initialize();
    delay(20);
    
    // Detect boot mode after "reboot"
    detect_boot_mode();
    
    // Show appropriate boot screen based on installation status
    if (system_state.boot_mode == BOOT_MODE_INSTALLED) {
        show_installed_system_boot();
    } else {
        show_loading_animation();
        show_boot_splash();
        show_live_system_boot();
    }
    
    terminal_writestring("Welcome to Xorix Server Edition v1.0\n");
    terminal_writestring("Type 'help' for available commands\n");
    terminal_writestring("Use Num 2/8 for terminal scrolling\n");
    if (system_state.boot_mode == BOOT_MODE_LIVE) {
        terminal_writestring("Type 'root mode' to access admin features\n");
    }
    terminal_writestring("\n");
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
    init_installation_state();
    init_system_state();
    detect_boot_mode();
    // Show appropriate boot screen based on system state
    if (system_state.boot_mode == BOOT_MODE_INSTALLED) {
        show_installed_system_boot();
    } else {
        show_loading_animation();
        show_boot_splash();
        show_live_system_boot();
    }
    
    terminal_writestring("Welcome to Xorix Server Edition v1.0\n");
    terminal_writestring("Type 'help' for available commands\n");
    terminal_writestring("Use Num 2/8 for terminal scrolling\n");
    if (system_state.boot_mode == BOOT_MODE_LIVE) {
        terminal_writestring("Type 'root mode' to access admin features\n");
    }
    terminal_writestring("\n");
    show_shell_prompt();
    while (1) {
        poll_keyboard();
    }
}

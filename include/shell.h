#ifndef SHELL_H
#define SHELL_H
#include "types.h"
#define MAX_INPUT_LENGTH 256
#define MAX_COMMAND_HISTORY 50
#define MAX_TERMINAL_HISTORY 1000
void show_shell_prompt();
void process_command(const char* command);
void init_shell();
void terminal_initialize();
void terminal_putchar(char c);
void terminal_writestring(const char* data);
void terminal_putentryat(char c, uint8_t color, size_t x, size_t y);
void terminal_write(const char* data, size_t size);
void scroll_terminal_up();
void scroll_terminal_down();
void add_to_terminal_history(const char* line);
void refresh_terminal_display();
void cmd_help();
void cmd_clear();
void cmd_about();
void cmd_version();
void cmd_uptime();
void cmd_memory();
void cmd_ls();
void cmd_cd(const char* path);
void cmd_pwd();
void cmd_mkdir(const char* dirname);
void cmd_rmdir(const char* dirname);
void cmd_rm(const char* filename);
void cmd_cp(const char* src, const char* dest);
void cmd_mv(const char* src, const char* dest);
void cmd_cat(const char* filename);
void cmd_echo(const char* text);
void cmd_touch(const char* filename);
void cmd_ps();
void cmd_kill(const char* pid);
void cmd_mount();
void cmd_whoami();
void cmd_users();
void cmd_adduser(const char* username);
void cmd_passwd();
void cmd_root_mode();
void cmd_server_mode();
void cmd_install_xorix();
extern char input_buffer[MAX_INPUT_LENGTH];
extern size_t input_length;
extern bool root_mode;
extern bool server_mode;
extern char terminal_history[MAX_TERMINAL_HISTORY][MAX_INPUT_LENGTH];
extern int terminal_history_count;
extern int terminal_scroll_offset;
extern bool scrolling_enabled;
#endif 

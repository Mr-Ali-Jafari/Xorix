#ifndef FILESYSTEM_H
#define FILESYSTEM_H
#include "types.h"
#define MAX_FILES 100
#define MAX_FILENAME 32
#define MAX_PATH 256
#define MAX_FILE_SIZE 1024
struct file_entry {
    char name[MAX_FILENAME];
    char path[MAX_PATH];
    char content[MAX_FILE_SIZE];
    bool is_directory;
    size_t size;
};
void init_filesystem();
bool create_file(const char* filename);
bool delete_file(const char* filename);
bool create_directory(const char* dirname);
bool delete_directory(const char* dirname);
struct file_entry* find_file(const char* filename);
void list_files();
bool write_file(const char* filename, const char* content);
bool read_file(const char* filename, char* buffer, size_t buffer_size);
extern struct file_entry filesystem[MAX_FILES];
extern int file_count;
extern char current_directory[MAX_PATH];
#endif 

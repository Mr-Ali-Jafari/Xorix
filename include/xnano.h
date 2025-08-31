#ifndef XNANO_H
#define XNANO_H
#include "types.h"
#define XNANO_MAX_LINES 50
#define XNANO_MAX_LINE_LENGTH 80
#define XNANO_MAX_FILENAME 32
void xnano_editor(const char* filename);
void xnano_display_content();
void xnano_handle_input(char key);
void xnano_save_file();
void xnano_load_file(const char* filename);
extern bool xnano_mode;
extern char xnano_filename[XNANO_MAX_FILENAME];
extern char xnano_content[XNANO_MAX_LINES][XNANO_MAX_LINE_LENGTH];
extern int xnano_line_count;
extern int xnano_cursor_line;
extern int xnano_cursor_col;
extern int xnano_scroll_offset;
extern bool xnano_modified;
#endif 

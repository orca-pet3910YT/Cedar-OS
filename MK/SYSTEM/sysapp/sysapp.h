#ifndef SYSAPP_H
#define SYSAPP_H

#include <stdint.h>

#define MK_LINE_MAX 128
#define MK_MAX_FILENAME 32
#define MK_MAX_FILE_SIZE 256
#define MK_MAX_FILES 16
    // MK sysapp for fallback dos mode, with some basic commands like
    // ver, print, sysinfo, cls, help, dir, reboot and MK32file 
    //(create a file in memory with content),
    // it shitty af
void MK_delay(int ms);
void MK_clear_screen(void);
void MK_vga_print(const char* str);
void MK_vga_backspace(void);
void MK_vga_print_char(char c);
void MK_vga_set_color(uint8_t color);
void MK_vga_reset_color(void);
void MK_sysapp_prompt(void);
int MK_sysapp_run(const char* line);

#endif
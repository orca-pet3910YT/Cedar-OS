#ifndef VGA_H
#define VGA_H

#include <stdint.h>

typedef uint16_t vga_entry_t;

#define VGA_WIDTH 80
#define VGA_HEIGHT 25

extern vga_entry_t* const VGA_BUFFER;

extern int cursor_x;
extern int cursor_y;

void MK_vga_clear_screen_impl(void);
void MK_vga_put_char_impl(char c);
void MK_vga_print_impl(const char* str);
void MK_vga_print_char_impl(char c);

void MK_vga_set_color_impl(uint8_t color);
uint8_t MK_vga_get_color_impl(void);
void MK_vga_reset_color_impl(void);

void MK_vga_delay_impl(int count);

#endif
/*
THIS IS A VERY BASIC VGA DRIVER FOR THE CEDAR OS MANGOKERNEL.
IT PROVIDES FUNCTIONS TO CLEAR THE SCREEN, PUT CHARACTERS AND STRINGS ON THE SCREEN,
AND A SIMPLE DELAY FUNCTION. THIS DRIVER USES THE VGA TEXT MODE,
WHICH ALLOWS US TO DISPLAY CHARACTERS WITH COLORS ON THE SCREEN.
THE VGA_BUFFER POINTS TO THE MEMORY LOCATION WHERE THE VGA TEXT MODE DATA IS STORED,
AND WE CAN WRITE TO THIS BUFFER TO DISPLAY CHARACTERS ON THE SCREEN.
 THE CURSOR_X AND CURSOR_Y VARIABLES KEEP TRACK OF THE CURRENT POSITION OF THE CURSOR ON THE SCREEN.


 --jad comment--

 *usable for only Cedar OS, not for any os. all rights reserved to jad 2026.
*/


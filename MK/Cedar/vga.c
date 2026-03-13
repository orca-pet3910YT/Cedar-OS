#include "vga.h"

vga_entry_t* const VGA_BUFFER = (vga_entry_t*) 0xB8000;
int cursor_x = 0;
int cursor_y = 0;

void clear_screen() {
    for (int y = 0; y < VGA_HEIGHT; y++) {
        for (int x = 0; x < VGA_WIDTH; x++) {
            const int index = y * VGA_WIDTH + x;
            VGA_BUFFER[index] = (vga_entry_t)(' ' | (0x07 << 8));
        }
    }
    cursor_x = 0;
    cursor_y = 0;
}

void vga_put_char(char c) {
    if (c == '\n') {
        cursor_x = 0;
        if (++cursor_y >= VGA_HEIGHT) {
            cursor_y = 0; // Wrap around the shit
        }
        return;
    }
    const int index = cursor_y * VGA_WIDTH + cursor_x;
    VGA_BUFFER[index] = (vga_entry_t)(c | (0x07 << 8));
    cursor_x++;
    if (cursor_x >= VGA_WIDTH) {
        cursor_x = 0;
        cursor_y++;
        if (cursor_y >= VGA_HEIGHT) {
            cursor_y = 0; // Piece of wrapping shit
        }
    }
}

void vga_put_string(const char* str) {
    while (*str) {
        vga_put_char(*str++);
    }
}

void delay(int count) {
    for (volatile int i = 0; i < count * 100000; i++) {
        // Busy wait
    }
}
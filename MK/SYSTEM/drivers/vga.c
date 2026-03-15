#include "vga.h"

vga_entry_t* const VGA_BUFFER = (vga_entry_t*) 0xB8000;
int cursor_x = 0;
int cursor_y = 0;
static uint8_t vga_color = 0x0F; // bright white on black

static inline vga_entry_t make_vga_entry(char c, uint8_t color) {
    return (vga_entry_t)c | (vga_entry_t)color << 8;
}

void MK_vga_set_color_impl(uint8_t color) {
    vga_color = color;
}

uint8_t MK_vga_get_color_impl(void) {
    return vga_color;
}

void MK_vga_reset_color_impl(void) {
    vga_color = 0x0F;
}

void MK_vga_clear_screen_impl() {
    for (int y = 0; y < VGA_HEIGHT; y++) {
        for (int x = 0; x < VGA_WIDTH; x++) {
            const int index = y * VGA_WIDTH + x;
            VGA_BUFFER[index] = make_vga_entry(' ', vga_color);
        }
    }
    cursor_x = 0;
    cursor_y = 0;
}

static void vga_scroll() {
    for (int y = 1; y < VGA_HEIGHT; y++) {
        for (int x = 0; x < VGA_WIDTH; x++) {
            VGA_BUFFER[(y - 1) * VGA_WIDTH + x] = VGA_BUFFER[y * VGA_WIDTH + x];
        }
    }
    for (int x = 0; x < VGA_WIDTH; x++) {
        VGA_BUFFER[(VGA_HEIGHT - 1) * VGA_WIDTH + x] = make_vga_entry(' ', vga_color);
    }
    cursor_y = VGA_HEIGHT - 1;
}

void MK_vga_put_char_impl(char c) {
    if (c == '\n') {
        cursor_x = 0;
        cursor_y++;
        if (cursor_y >= VGA_HEIGHT) {
            vga_scroll();
        }
        return;
    }
    if (c == '\r') {
        cursor_x = 0;
        return;
    }
    const int index = cursor_y * VGA_WIDTH + cursor_x;
    VGA_BUFFER[index] = make_vga_entry(c, vga_color);
    cursor_x++;
    if (cursor_x >= VGA_WIDTH) {
        cursor_x = 0;
        cursor_y++;
        if (cursor_y >= VGA_HEIGHT) {
            vga_scroll();
        }
    }
}

void MK_vga_print_char_impl(char c) {
    MK_vga_put_char_impl(c);
}

void MK_vga_print_impl(const char* str) {
    while (*str) {
        MK_vga_put_char_impl(*str++);
    }
}

void MK_vga_delay_impl(int count) {
    for (volatile int i = 0; i < count * 100000; i++) {
        /*
        This little f*cker cant work...
        - jad 18:00 13 march 2026



        ok it work
        -jad 16:14 14 march 2026
        */
    }
}
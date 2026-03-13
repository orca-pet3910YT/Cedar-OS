#include "vga.h"

void kernel_main() {
    delay(2000);
    clear_screen();
    delay(980);
    vga_put_string(".-----------------------------------------------------------------------------.\n");
    vga_put_string("|                             Cedar OS MangoKernel                            |\n");
    delay(100);
    vga_put_string("|                              COPYRIGHT JAD 2026                             |\n");
    vga_put_string("'-----------------------------------------------------------------------------'\n");
    delay(1000);
    vga_put_string("Loading Cedar...\n");
    vga_put_string("\n");
    while(1);
}
#include "sysapp/sysapp.h"
#include "drivers/vga.h"

void MK_delay(int ms) {
    MK_vga_delay_impl(ms);
}

void MK_clear_screen() {
    MK_vga_clear_screen_impl();
}

void MK_vga_print(const char* str) {
    MK_vga_print_impl(str);
}

void MK_vga_set_color(uint8_t color) {
    MK_vga_set_color_impl(color);
}

void MK_vga_reset_color(void) {
    MK_vga_reset_color_impl();
}

void MK_vga_backspace() {
    if (cursor_x > 0) {
        cursor_x--;
    } else if (cursor_y > 0) {
        cursor_y--;
        cursor_x = VGA_WIDTH - 1;
    } else {
        return;
    }
    int index = cursor_y * VGA_WIDTH + cursor_x;
    VGA_BUFFER[index] = (vga_entry_t)(' ' | ((uint16_t)MK_vga_get_color_impl() << 8));
}

void MK_vga_print_char(char c) {
    MK_vga_print_char_impl(c);
}

static int str_eq(const char *a, const char *b) {
    while (*a && *b && *a == *b) { a++; b++; }
    return *a == *b;
}

static int str_eq_ci(const char *a, const char *b) {
    while (*a && *b) {
        char ca = *a;
        char cb = *b;
        if (ca >= 'A' && ca <= 'Z') ca += 'a' - 'A';
        if (cb >= 'A' && cb <= 'Z') cb += 'a' - 'A';
        if (ca != cb) return 0;
        a++; b++;
    }
    return *a == *b;
}

static inline unsigned char MK_inb(unsigned short port) {
    unsigned char r;
    __asm__ __volatile__ ("inb %1, %0" : "=a"(r) : "dN"(port));
    return r;
}

static inline void MK_outb(unsigned short port, unsigned char v) {
    __asm__ __volatile__ ("outb %0, %1" :: "a"(v), "dN"(port));
}

static unsigned int MK_cmos_read(unsigned char addr) {
    MK_outb(0x70, addr | 0x80);
    return (unsigned int)MK_inb(0x71);
}

static void MK_print_uint(unsigned int n) {
    char buf[12];
    int i = 0;
    if (n == 0) {
        MK_vga_print_char('0');
        return;
    }
    while (n) {
        buf[i++] = '0' + (n % 10);
        n /= 10;
    }
    while (i > 0) MK_vga_print_char(buf[--i]);
}

static void cmd_ver(void) {
    MK_vga_print("Cedar OS [Version 0.3.14 BETA, Build 450]\n");
    MK_vga_print("Fallback Kernel - Copyright (c) 2026 Jad\n");
    MK_vga_print("Open-source software licensed under GNU.\n");
}


static void cmd_MKclear(void) {
    MK_clear_screen();
}

static void cmd_print(const char *rest) {
    if (rest && *rest) MK_vga_print(rest);
    MK_vga_print("\n");
}

static void cmd_sysinfo(void) {
    MK_vga_print("-- CPU --\n");
    {
        unsigned int ebx, ecx, edx;
        __asm__ __volatile__ (
            "cpuid"
            : "=b"(ebx), "=c"(ecx), "=d"(edx)
            : "a"(0)
        );
        char v[13];
        v[0] = (char)(ebx & 0xff);
        v[1] = (char)((ebx >> 8) & 0xff);
        v[2] = (char)((ebx >> 16) & 0xff);
        v[3] = (char)((ebx >> 24) & 0xff);
        v[4] = (char)(ecx & 0xff);
        v[5] = (char)((ecx >> 8) & 0xff);
        v[6] = (char)((ecx >> 16) & 0xff);
        v[7] = (char)((ecx >> 24) & 0xff);
        v[8] = (char)(edx & 0xff);
        v[9] = (char)((edx >> 8) & 0xff);
        v[10] = (char)((edx >> 16) & 0xff);
        v[11] = (char)((edx >> 24) & 0xff);
        v[12] = '\0';
        MK_vga_print("Vendor: ");
        MK_vga_print(v);
        MK_vga_print("\n");
    }
    MK_vga_print("-- RAM --\n");
    {
        unsigned int low = MK_cmos_read(0x17) | (MK_cmos_read(0x18) << 8);
        unsigned int high = MK_cmos_read(0x30) | (MK_cmos_read(0x31) << 8);
        unsigned int total_kb = low + high;
        if (total_kb == 0) total_kb = low;
        unsigned int total_mb = total_kb / 1024;
        MK_vga_print("Extended: ");
        MK_print_uint(total_kb);
        MK_vga_print(" KB (");
        MK_print_uint(total_mb);
        MK_vga_print(" MB)\n");
    }
    MK_vga_print("-- Storage --\n");
    MK_vga_print("Boot device: floppy / disk 0\n");
}

static void cmd_help(void) {
    MK_vga_print("======================================================\n");
    MK_vga_print("ver               ||    version\n");
    MK_vga_print("print <text>      ||    echo text\n");
    MK_vga_print("sysinfo           ||    system info (CPU, RAM, storage)\n");
    MK_vga_print("help              ||    this list\n");
    MK_vga_print("about             ||    information about the OS and authors\n");
    MK_vga_print("dir               ||    list files\n");
    MK_vga_print("MKcls             ||    clear screen\n");
    MK_vga_print("reboot            ||    reboot machine\n");
    MK_vga_print("======================================================\n");
}

static void cmd_about(void) {
    MK_clear_screen();
    MK_vga_set_color(0x0F);
    MK_vga_print("Cedar OS - Liteweight small OS, [fallback mode] powered by MangoKernel1 (MK1)\n");
    MK_vga_print("Made because idk i wanted it.\n");
    MK_vga_print("Creators: JAD and ORCA\n");
    MK_vga_print("Commands: help, MKboot, about, sysinfo, dir, MKcls, reboot.\n");
    MK_vga_reset_color();
}

static void cmd_dir(void) {
    MK_vga_print("No file system available in this fallback mode.\n");
}

static void cmd_MKboot(void) {
    MK_vga_print("Booting into normal Mode...\n");
        MK_vga_print("[=*****************]\n");
        MK_delay(100);
        MK_clear_screen();
        MK_vga_print("[====**************]\n");
        MK_delay(100);
        MK_clear_screen();
        MK_vga_print("[========**********]\n");
        MK_delay(100);
        MK_clear_screen();
        MK_vga_print("[============******]\n");
        MK_delay(100);
        MK_clear_screen();
        MK_vga_print("[========**********]\n");
        MK_clear_screen();
        MK_delay(100);
        MK_vga_print("[============******]\n");
        MK_clear_screen();
        MK_delay(100);
        MK_clear_screen();
        MK_vga_print("[===============***]\n");
    MK_delay(4000);
    MK_clear_screen();
    MK_delay(1000);
    MK_vga_set_color(0x0C);
    MK_vga_print("[CRITICAL ERROR] FAILURE TO SWITCH TO NORMAL MODE.\n");
    MK_vga_print("Error 0x000000a1\n");
    MK_vga_reset_color();

    // 0x000000a1 failure to find system necessary files for normal mode, AND this make fucking sense
    /*
    The reason for this error is that the fallback dos mode does not have 
    access to the normal mode files, and this is intentional, 
    because if the fallback dos mode didnt found the files


    we are working to make, but me, JAD, i dont have any time for this as school is taking all my time, 
    and i want to release the os as soon as possible,
    and the fallback dos mode is already a good fallback for users who want to use the os without normal mode,
    and the os is early


    let give some time to be developed, and maybe in the future we will add a way to switch to normal mode
    from fallback dos mode, but for now, this is the best we can do.




    error list:


    0x000000a1 - failure to find system necessary files for normal mode
    0x000000a2 - failure to load kernel
    0x000000a3 - failure to load graphism or keyboard drivers
    0x000000a4 - failure to initialize idt
    0x000000a5 - Command interperter critical failure

    */
}

void MK_sysapp_prompt(void) {
    MK_vga_print("S:/BOOT/fallback>");
}

static void MK_load_bugcheck_file(void) {
    MK_clear_screen();
    MK_vga_set_color(0x1F); // white on blue

    for (int j = 0; j < 10; j++) {
        MK_vga_print("\n");
    }

    for (int i = 5; i >= 0; i--) {
        MK_clear_screen();
        MK_vga_set_color(0x1F);
        for (int j = 0; j < 10; j++) {
            MK_vga_print("\n");
        }
    MK_vga_print("                 "); MK_vga_print("  ####    ######    \n");
    MK_vga_print("                 "); MK_vga_print("  ####    ##        \n");
    MK_vga_print("                 "); MK_vga_print("          ##       \n");
    MK_vga_print("                 "); MK_vga_print("          ##        \n");
    MK_vga_print("                 "); MK_vga_print("  ####    ##        \n");
    MK_vga_print("                 "); MK_vga_print("  ####    ######  \n\n\n\n\n\n");
        MK_vga_print("                CEDAR OS KERNEL HAS DETECTED A CRITICAL FAILURE AND IS CRASHING\n");
        MK_vga_print("                CEDAR OS WILL REBOOT IN ");
        MK_print_uint(i);
        MK_vga_print(" seconds...\n");
        MK_delay(1000);
    }

    // attempt reboot
    __asm__ __volatile__("cli; movb $0xFE, %al; outb %al, $0x64; hlt");
}

void MKbooth(void) {
    MK_vga_print("USAGE =====================================================\n");
    MK_vga_print("MKboot -h : Show help\n");
    MK_vga_print("MKboot -n : Boot into normal mode\n");
    MK_vga_print("MKboot -c -f : Bug kernel panic check (load BUGCHECK and simulate triple fault)\n");
}

int MK_sysapp_run(const char *line) {
    int i = 0;
    while (line[i] == ' ') i++;
    int cmd_start = i;
    while (line[i] && line[i] != ' ') i++;
    if (i <= cmd_start) return 0;
    if (str_eq_ci("ver", line + cmd_start)) {
        cmd_ver();
        return 1;
    }
    if (str_eq_ci("version", line + cmd_start)) {
        cmd_ver();
        return 1;
    }
    if (str_eq_ci("print", line + cmd_start)) {
        while (line[i] == ' ') i++;
        cmd_print(line + i);
        return 1;
    }
    if (str_eq_ci("sysinfo", line + cmd_start)) {
        cmd_sysinfo();
        return 1;
    }
    if (str_eq_ci("cls", line + cmd_start) || str_eq_ci("clear", line + cmd_start) || str_eq_ci("MKcls", line + cmd_start)) {
        cmd_MKclear();
        return 1;
    }
    if (str_eq_ci("help", line + cmd_start)) {
        cmd_help();
        return 1;
    }
        if (str_eq_ci("MKboot -h", line + cmd_start)) {
        MKbooth();
        return 1;
    }
    if (str_eq_ci("MKboot -n", line + cmd_start)) {
        cmd_MKboot();
        return 1;
    }
    if (str_eq_ci("MKboot -c -f", line + cmd_start) || str_eq_ci("MKboot -c", line + cmd_start)) {
        MK_load_bugcheck_file();
        return 1;
    }
    if (str_eq_ci("dir", line + cmd_start)) {
        cmd_dir();
        return 1;
    }
    if (str_eq_ci("reboot", line + cmd_start)) {
        MK_vga_print("Rebooting...\n");
        __asm__ __volatile__("cli; movb $0xFE, %al; outb %al, $0x64; hlt");
        return 1;
    }
    if (str_eq_ci("about", line + cmd_start) || str_eq_ci("MKabout", line + cmd_start)) {
        cmd_about();
        return 1;
    }
    return 0;
}

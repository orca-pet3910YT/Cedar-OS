#include "idt.h"
#define IDT_ENTRIES 256
static idt_entry_t idt[IDT_ENTRIES];
static idt_ptr_t idt_ptr;

// fallback int hand
void isr_default(void) {
    // Halt forever on a annoying little int f*cker.
    while (1) {
        __asm__ __volatile__("hlt");
    }
}
static void idt_set_gate(uint8_t num, uint32_t base, uint16_t sel, uint8_t flags) {
    idt[num].base_low = base & 0xFFFF;
    idt[num].sel = sel;
    idt[num].always0 = 0;
    idt[num].flags = flags;
    idt[num].base_high = (base >> 16) & 0xFFFF;
}
void init_idt(void) {
    idt_ptr.limit = (sizeof(idt_entry_t) * IDT_ENTRIES) - 1;
    idt_ptr.base = (uint32_t)&idt;
    for (int i = 0; i < IDT_ENTRIES; i++) {
        idt_set_gate(i, (uint32_t)isr_default, 0x08, 0x8E);
    }
    __asm__ __volatile__("lidt (%0)" :: "r"(&idt_ptr));
    __asm__ __volatile__("sti");
    // people using asm in the big 2026...
}
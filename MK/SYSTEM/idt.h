#ifndef IDT_H
#define IDT_H

// define this bullsh*t
#include <stdint.h>
typedef struct {
    uint16_t base_low;
    uint16_t sel;
    uint8_t always0;
    uint8_t flags;
    uint16_t base_high;
    // little idt fucker
} __attribute__((packed)) idt_entry_t;
typedef struct {
    uint16_t limit;
    uint32_t base;
    // now idt got a structure while me i do not have a physical structure nor a emotional one..
    // f*ck this shit.
} __attribute__((packed)) idt_ptr_t;
void init_idt(void);
#endif
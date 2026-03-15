[BITS 32]
global _start
extern kernel_main
; kernel entry for cedarkernel
; kernel shit
_start:
    mov ax, 0x10
    mov ds, ax
    mov ss, ax
    mov esp, 0x90000
    call kernel_main
    cli
    hlt

    ; Mk is Merde kernel
    ; f*ck gcc
    ; f*ck nasm
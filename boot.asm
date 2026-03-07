[org 0x7C00]
[bits 16]
CODE_SEG equ gdt_code - gdt_start
DATA_SEG equ gdt_data - gdt_start
infosysbtl:
    mov bx, cedar_msg
    call print_service
    mov bx, KernelLOADmsg
    call print_service
    mov bx, msg_32b
    call print_service
    cli
    lgdt [gdt_descriptor]
    mov eax, cr0
    or eax, 1
    mov cr0, eax
    jmp CODE_SEG:init_pm
print_service:
    mov al, [bx]
    cmp al, 0
    je done
    mov ah, 0x0E
    int 0x10
    add bx, 1
    jmp print_service
done:
    ret
cedar_msg db "CEDAR OS BOOTLOADER", 0x0D, 0x0A, \
             "======================================", 0x0D, 0x0A, 0

msg_32b db "[OK] Switched to 32bit.", 0x0D, 0x0A, 0
KernelLOADmsg db "[...] Loading Kernel...", 0x0D, 0x0A, 0
gdt_start:
    dw 0x0, 0x0, 0x0, 0x0
gdt_code:
    dw 0xffff
    dw 0x0
    db 0x0
    db 10011010b
    db 11001111b
    db 0x0
gdt_data:
    dw 0xffff
    dw 0x0
    db 0x0
    db 10010010b
    db 11001111b
    db 0x0
gdt_end:
gdt_descriptor:
    dw gdt_end - gdt_start - 1
    dd gdt_start
[bits 32]
init_pm:
    mov ax, DATA_SEG
    mov ds, ax
    mov ss, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    mov ebp, 0x90000
    mov esp, ebp
    jmp $
times 510-($-$$) db 0
dw 0xAA55
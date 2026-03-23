[BITS 16]
[ORG 0x7C00]
CODE_SEG equ gdt_code - gdt_start
DATA_SEG equ gdt_data - gdt_start
mov ax, 0x3
int 0x10
mov ah, 0x02
mov bh, 0
mov dh, 1
mov dl, 0
int 0x10
mov si, test1
call print_string
mov ah, 0x02
mov bh, 0
mov dh, 3
mov dl, 0
int 0x10
mov si, test2
call print_string
mov ah, 0x02
mov bh, 0
mov dh, 5
mov dl, 0
int 0x10
mov ah, 0x09
mov al, '='
mov bh, 0
mov bl, 0x0F
mov cx, 80
int 0x10
mov ah, 0x02
mov bh, 0
mov dh, 6
mov dl, 0
int 0x10
mov si, msg
call print_string
mov ah, 0x02
mov bh, 0
mov dh, 7
mov dl, 0
int 0x10
mov ah, 0x09
mov al, '='
mov bh, 0
mov bl, 0x0F
mov cx, 80
int 0x10
mov ah, 0x02
mov bh, 0
mov dh, 9
mov dl, 0
int 0x10
mov si, test3
call print_string
mov ah, 0x02
mov al, 30
mov ch, 0
mov cl, 2
mov dh, 0
mov dl, 0x00
mov bx, 0x1000
mov es, bx
xor bx, bx
int 0x13
jc disk_err_label
disk_ok:
cli
mov al, 0xFF
out 0x21, al
out 0xA1, al
lgdt [gdt_descriptor]
mov eax, cr0
or eax, 1
mov cr0, eax
jmp CODE_SEG:init_pm
disk_err_label:
mov si, disk_err
call print_string
jmp $
print_string:
lodsb
cmp al, 0
je .done
mov ah, 0x0E
mov bh, 0
int 0x10
jmp print_string
.done:
ret
msg: db "CEDAR OS MASTER BOOT RECORD BOOTLOADER FOR 86X", 0
test1: db "[..] init system...", 0
test2: db "[OK] diskreadmgr", 0
test3: db "[OK] Reading disk for Kernel", 0
disk_err: db "[ERROR] 0x000000a2: No kernel found", 0
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
[BITS 32]
init_pm:
mov ax, DATA_SEG
mov ds, ax
mov ss, ax
mov es, ax
mov fs, ax
mov gs, ax
mov ebp, 0x90000
mov esp, ebp
jmp CODE_SEG:0x10000
times 510 - ($ - $$) db 0
dw 0xAA55
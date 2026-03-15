#!/bin/bash

INCLUDE_PATH="./SYSTEM"
BUILD_PATH="./build"
KERNEL_ENTRY_ASM="./Cedar/kernel_entry.asm"
KERNEL_C="./Cedar/MKkernel.c"

# Create build directory if not exists
mkdir -p "$BUILD_PATH"

# Assemble kernel_entry.asm to object
nasm -f elf32 "$KERNEL_ENTRY_ASM" -o "$BUILD_PATH/kernel_entry.o"

# Compile kernel C code
gcc -m32 -ffreestanding -fno-builtin -nostdlib -I"$INCLUDE_PATH" -c "$KERNEL_C" -o "$BUILD_PATH/kernel.o"

# Compile other sources
gcc -m32 -ffreestanding -fno-builtin -nostdlib -I"$INCLUDE_PATH" -c "$INCLUDE_PATH/idt.c" -o "$BUILD_PATH/idt.o"
gcc -m32 -ffreestanding -fno-builtin -nostdlib -I"$INCLUDE_PATH" "$INCLUDE_PATH/sysapp/sysapp.c" -c -o "$BUILD_PATH/sysapp.o"
gcc -m32 -ffreestanding -fno-builtin -nostdlib -I"$INCLUDE_PATH" "$INCLUDE_PATH/drivers/vga.c" -c -o "$BUILD_PATH/vga.o"
gcc -m32 -ffreestanding -fno-builtin -nostdlib -I"$INCLUDE_PATH" "$INCLUDE_PATH/drivers/keyboard.c" -c -o "$BUILD_PATH/keyboard.o"

# Link all object files into kernel binary
ld -m elf_i386 -T "$INCLUDE_PATH/linker.ld" -o "$BUILD_PATH/kernel.bin" \
  "$BUILD_PATH/kernel_entry.o" \
  "$BUILD_PATH/kernel.o" \
  "$BUILD_PATH/idt.o" \
  "$BUILD_PATH/sysapp.o" \
  "$BUILD_PATH/vga.o" \
  "$BUILD_PATH/keyboard.o"

# Create bootable ISO or raw image (assuming boot.asm prepares boot sector)
# For simplicity, just copy boot.bin if exists, or use a custom approach
# Example: create a floppy image with boot.bin and kernel.bin
cat "$BUILD_PATH/boot.bin" "$BUILD_PATH/kernel.bin" > "$BUILD_PATH/boot.img"

echo "Build complete. Boot image at $BUILD_PATH/boot.img"
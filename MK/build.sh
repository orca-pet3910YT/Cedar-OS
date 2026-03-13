#!/bin/bash

set -e

BUILD_DIR="./build"
BOOT_DIR="./Boot"
CEDAR_DIR="./Cedar"
IMG_NAME="cedar.img"

mkdir -p "$BUILD_DIR"

echo "=== Building Cedar OS ==="

echo "[1/4] Compiling bootloader..."
nasm "$BOOT_DIR/boot.asm" -o "$BUILD_DIR/boot.bin"

echo "[2/4] Compiling kernel..."
if command -v i686-elf-gcc &> /dev/null; then
    nasm "$CEDAR_DIR/kernel_entry.asm" -f elf -o "$BUILD_DIR/kernel_entry.o"
    i686-elf-gcc -m32 -ffreestanding -fno-builtin -nostdlib -c "$CEDAR_DIR/MKkernel.c" -o "$BUILD_DIR/kernel.o"
    # Compile vga.c
    i686-elf-gcc -m32 -ffreestanding -fno-builtin -nostdlib -c "$CEDAR_DIR/vga.c" -o "$BUILD_DIR/vga.o"
elif command -v gcc &> /dev/null; then
    nasm "$CEDAR_DIR/kernel_entry.asm" -f elf -o "$BUILD_DIR/kernel_entry.o"
    gcc -m32 -ffreestanding -fno-builtin -nostdlib -c "$CEDAR_DIR/MKkernel.c" -o "$BUILD_DIR/kernel.o"
    # Compile vga.c
    gcc -m32 -ffreestanding -fno-builtin -nostdlib -c "$CEDAR_DIR/vga.c" -o "$BUILD_DIR/vga.o"
else
    echo "Error: gcc or i686-elf-gcc not found"
    exit 1
fi

cat > "$BUILD_DIR/linker.ld" <<EOF
ENTRY(kernel_main)

SECTIONS
{
  . = 0x1000;
  .text : { *(.text*) }
  .data : { *(.data*) }
  .bss : { *(.bss*) }
}
EOF

echo "[3/4] Linking kernel with inline linker script..."
if command -v i686-elf-ld &> /dev/null; then
    i686-elf-ld -m elf_i386 -T "$BUILD_DIR/linker.ld" -o "$BUILD_DIR/kernel.elf" "$BUILD_DIR/kernel_entry.o" "$BUILD_DIR/kernel.o" "$BUILD_DIR/vga.o"
elif command -v ld &> /dev/null; then
    ld -m elf_i386 -T "$BUILD_DIR/linker.ld" -o "$BUILD_DIR/kernel.elf" "$BUILD_DIR/kernel_entry.o" "$BUILD_DIR/kernel.o" "$BUILD_DIR/vga.o"
else
    echo "Error: linker not found"
    exit 1
fi

echo "[4/4] Converting ELF to binary..."
if command -v i686-elf-objcopy &> /dev/null; then
    i686-elf-objcopy -O binary "$BUILD_DIR/kernel.elf" "$BUILD_DIR/kernel.bin"
elif command -v objcopy &> /dev/null; then
    objcopy -O binary "$BUILD_DIR/kernel.elf" "$BUILD_DIR/kernel.bin"
else
    echo "Error: objcopy not found"
    exit 1
fi

echo "[5/5] Merging bootloader and kernel..."
cat "$BUILD_DIR/boot.bin" "$BUILD_DIR/kernel.bin" > "$BUILD_DIR/$IMG_NAME"

echo "[6/6] Padding image to 1.44MB (floppy size)..."
truncate -s 1474560 "$BUILD_DIR/$IMG_NAME"

echo ""
echo "=== Build Complete ==="
echo "Output: $BUILD_DIR/$IMG_NAME ($(du -h "$BUILD_DIR/$IMG_NAME" | cut -f1))"
echo ""
echo "Test with QEMU:"
echo "  qemu-system-i386 build/cedar.img"
echo "  qemu-system-i386 -fda build/cedar.img"
echo "  qemu-system-i386 -hda build/cedar.img -boot c"
#!/bin/bash

set -e

BUILD_DIR="./build"
BOOT_DIR="./Boot"
CEDAR_DIR="./Cedar"
SYSTEM_DIR="./SYSTEM"
IMG_NAME="cedar.img"

mkdir -p "$BUILD_DIR"

echo "=== Building Cedar OS ==="

echo "[1/4] Compiling bootloader..."
nasm "$BOOT_DIR/boot.asm" -o "$BUILD_DIR/boot.bin"

echo "Cedar directory: $CEDAR_DIR"
ls "$CEDAR_DIR"

echo "[2/4] Compiling kernel..."

if [ ! -f "$CEDAR_DIR/kernel_entry.asm" ]; then
    echo "Error: $CEDAR_DIR/kernel_entry.asm not found!"
    exit 1
fi

nasm "$CEDAR_DIR/kernel_entry.asm" -f elf -o "$BUILD_DIR/kernel_entry.o"

INCLUDE_PATH="$PWD/SYSTEM"

if command -v i686-elf-gcc &> /dev/null; then
    echo "Using i686-elf-gcc..."
    gcc_cmd="i686-elf-gcc"
    ld_cmd="i686-elf-ld"
    objcopy_cmd="i686-elf-objcopy"
elif command -v gcc &> /dev/null; then
    echo "Using gcc..."
    gcc_cmd="gcc"
    ld_cmd="ld"
    objcopy_cmd="objcopy"
else
    echo "Error: gcc or i686-elf-gcc not found"
    exit 1
fi

echo "[2.1] Compiling MKkernel.c..."
$gcc_cmd -m32 -ffreestanding -fno-builtin -nostdlib -I"$INCLUDE_PATH" -c "$CEDAR_DIR/MKkernel.c" -o "$BUILD_DIR/kernel.o"

echo "[2.2] Compiling vga.c..."
$gcc_cmd -m32 -ffreestanding -fno-builtin -nostdlib -I"$INCLUDE_PATH" -c "$SYSTEM_DIR/drivers/vga.c" -o "$BUILD_DIR/vga.o"

echo "[2.3] Compiling keyboard.c..."
$gcc_cmd -m32 -ffreestanding -fno-builtin -nostdlib -I"$INCLUDE_PATH" -c "$SYSTEM_DIR/drivers/keyboard.c" -o "$BUILD_DIR/keyboard.o"

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

echo "[3/4] Linking kernel..."
$ld_cmd -m elf_i386 -T "$BUILD_DIR/linker.ld" -o "$BUILD_DIR/kernel.elf" "$BUILD_DIR/kernel_entry.o" "$BUILD_DIR/kernel.o" "$BUILD_DIR/vga.o" "$BUILD_DIR/keyboard.o"

echo "[4/4] Converting ELF to binary..."
$objcopy_cmd -O binary "$BUILD_DIR/kernel.elf" "$BUILD_DIR/kernel.bin"

echo "[5/5] Merging bootloader and kernel..."
cat "$BUILD_DIR/boot.bin" "$BUILD_DIR/kernel.bin" > "$BUILD_DIR/$IMG_NAME"

echo "[6/6] Padding image to 1.44MB..."
echo "[6/6] Padding image to 1.44MB..."
echo "[6/6] Padding image to 1.44MB..."
echo "[6/6] Padding image to 1.44MB..."
echo "[6/6] Padding image to 1.44MB..."
echo "[6/6] Padding image to 1.44MB..."
echo "[6/6] Padding image to 1.44MB..."
echo "[6/6] Padding image to 1.44MB..."
echo "[6/6] Padding image to 1.44MB..."
echo "[6/6] Padding image to 1.44MB..."
truncate -s 1474560 "$BUILD_DIR/$IMG_NAME"

echo "[6/6] Padding image to 1.44MB..."
echo "[6/6] Padding image to 1.44MB..."
echo "[6/6] Padding image to 1.44MB..."
echo "[6/6] Padding image to 1.44MB..."
echo "[6/6] Padding image to 1.44MB..."
echo "[6/6] Padding image to 1.44MB..."
echo "[6/6] Padding image to 1.44MB..."
echo "[6/6] Padding image to 1.44MB..."
echo "[6/6] Padding image to 1.44MB..."
echo "[6/6] Padding image to 1.44MB..."
echo ""
echo "=== Build Complete ==="
echo "Output: $BUILD_DIR/$IMG_NAME ($(du -h "$BUILD_DIR/$IMG_NAME" | cut -f1))"
echo ""
echo "Test with QEMU:"
echo "  qemu-system-i386 build/$IMG_NAME"
echo "  qemu-system-i386 -fda build/$IMG_NAME"
echo "  qemu-system-i386 -hda build/$IMG_NAME -boot c"
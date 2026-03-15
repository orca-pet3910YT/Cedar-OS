ASM=nasm
CC=gcc
LD=ld
QEMU=qemu-system-i386

CFLAGS=-m32 -ffreestanding -nostdlib -fno-pie -fno-stack-protector
LDFLAGS=-m elf_i386 -Ttext 0x10000 --oformat binary

all: os.img

boot.bin: Boot/boot.asm
	nasm Boot/boot.asm -o boot.bin

kernel.o: Cedar/MKkernel.c
	gcc $(CFLAGS) -c Cedar/MKkernel.c -o kernel.o

kernel.bin: kernel.o
	ld $(LDFLAGS) kernel.o -o kernel.bin

os.img: boot.bin kernel.bin
	cat boot.bin kernel.bin > os.img

run: os.img
	qemu-system-i386 -drive format=raw,file=os.img

clean:
	rm -f *.bin *.o *.img

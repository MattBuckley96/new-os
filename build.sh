nasm -f bin stage1.asm -o stage1.bin
nasm -f bin stage2.asm -o stage2.bin
cat stage1.bin stage2.bin > boot.bin

nasm -f elf32 kernel_entry.asm -o kernel_entry.o
clang --target=i386-elf-none-pc -m32 -c kernel.c -o kernel.o
ld -m elf_i386 --oformat binary -Tkernel.ld -nostdlib kernel_entry.o kernel.o -o kernel.bin
cat boot.bin kernel.bin > new_os.bin

rm -f boot.bin kernel.bin stage1.bin stage2.bin *.o

qemu-system-i386 new_os.bin -m 256
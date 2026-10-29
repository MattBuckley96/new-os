bits 16
org 0x7C00

CODE_SEG equ gdtCode - gdtStart
DATA_SEG equ gdtData - gdtStart
KERNEL_ENTRY_ADDRESS equ 1000

boot:
	jmp start

start:
	mov [bootDrive], dl

	cli
	xor ax, ax
	mov es, ax
	mov ds, ax
	mov ss, ax
	mov sp, 0x7C00
	sti

	; set video mode
	mov ax, 0x0013
	int 0x10
	
	; load kernel off disk
	mov ah, 0x42
	mov dl, [bootDrive]
	mov si, diskAddressPacket
	int 0x13

	; load gdt
	cli
	lgdt [gdtDesc]
	
	mov eax, cr0
	or eax, 1
	mov cr0, eax

	jmp CODE_SEG:start32

bootDrive:
	db 0

gdtStart:
    gdtNull:
        dd 0
        dd 0
    gdtCode:
        dw 0xFFFF
        dw 0x0
        db 0x0
        db 0b10011010
        db 0b11001111
        db 0x0
    gdtData:
        dw 0xFFFF
        dw 0x0
        db 0x0
        db 0b10010010
        db 0b11001111
        db 0x0
gdtEnd:

gdtDesc:
    dw gdtEnd - gdtStart - 1
    dd gdtStart

diskAddressPacket:
.size:
	db 0x10
.zero:
	db 0
.sectorCount:
	dw 2
.bufferSegment:
	dw KERNEL_ENTRY_ADDRESS
.bufferOffset:
	dw 0
.address:
	dq 2

bits 32

start32:
    mov ax, DATA_SEG
    mov ds, ax
    mov ss, ax
    mov es, ax
    mov fs, ax
    mov gs, ax

	jmp KERNEL_ENTRY_ADDRESS

stop:
	hlt
	jmp stop

times 510-($-$$) db 0
dw 0xAA55
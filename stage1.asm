bits 16
org 0x7C00

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

	; load stage2 off disk
	mov ah, 0x42
	mov dl, [bootDrive]
	mov si, diskAddressPacket
	int 0x13
	
	push dx
	jmp 0x7E00

bootDrive:
	db 0
	
diskAddressPacket:
.size:
	db 0x10
.zero:
	db 0
.sectorCount:
	dw 2
.bufferSegment:
	dw 0x7E00
.bufferOffset:
	dw 0
.address:
	dq 1

times 510-($-$$) db 0
dw 0xAA55
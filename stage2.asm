bits 16
org 0x7E00

CODE_SEG equ gdtCodeSegment - gdtStart
DATA_SEG equ gdtDataSegment - gdtStart
KERNEL_ENTRY_ADDRESS equ 10000
VIDEO_WIDTH equ 1280
VIDEO_HEIGHT equ 720
BPP equ 32

start:
	pop dx
	mov [bootDrive], dl

get_vbe_info:
	mov ax, 0x4F00
	mov di, vbeInfoBlock
	int 0x10
	cmp ax, 0x004F
	je .no_error
	mov si, getVbeInfoErrorMessage
.error:
	call print
	jmp stop
.no_error:
 
find_suitable_video_mode:
	; get video mode from array
	mov ax, [vbeInfoBlock.videoModeSegment]
	mov fs, ax
	mov di, [vbeInfoBlock.videoModeOffset]
.next_video_mode:
	mov cx, [fs:di]
	add di, 2
	cmp cx, 0xFFFF
	je .error

	; get video mode info
	push di
	mov ax, 0x4F01
	mov di, videoModeInfo
	int 0x10
	pop di
	cmp ax, 0x004F
	jne .error

	mov ax, [videoModeInfo.videoWidth]
	cmp ax, VIDEO_WIDTH
	jne .next_video_mode
	
	mov ax, [videoModeInfo.videoHeight]
	cmp ax, VIDEO_HEIGHT
	jne .next_video_mode

	mov al, [videoModeInfo.bitsPerPixel]
	cmp al, BPP
	jne .next_video_mode

	; found a video mode, now set it
	mov ax, 0x4F02
	mov bx, cx
	int 0x10
	cmp ax, 0x004F
	je .done
.error:
	mov si, getVideoModeInfoErrorMessage
	call print
	jmp stop
.done:

	; load kernel off disk
	mov ah, 0x42
	mov dl, [bootDrive]
	mov si, diskAddressPacket
	int 0x13
	
set_a20:
	in al, 0x92
	test al, 2
	jnz .done
	or al, 2
	and al, 0xFE
	out 0x92, al
.done:

	; load gdt
	cli
	lgdt [gdtDesc]
	
	mov eax, cr0
	or eax, 1
	mov cr0, eax

	jmp CODE_SEG:start32

print:
	mov ah, 0x0E
.loop:
	lodsb
	cmp al, 0
	je .done
	int 0x10
	jmp .loop
.done:
	ret

stop:
	hlt
	jmp stop

getVbeInfoErrorMessage: db "Failed to get VBE Info!", 0
getVideoModeInfoErrorMessage: db "Failed to get video mode!", 0
bootDrive: db 0

gdtStart:
gdtNullSegment:
	dd 0
	dd 0
gdtCodeSegment:
	dw 0xFFFF
	dw 0x0 
	db 0x0
	db 0b10011010
	db 0b11001111
	db 0x0
gdtDataSegment:
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
.size: db 0x10	
.zero: db 0
.sectorCount: dw 3
.bufferSegment: dw KERNEL_ENTRY_ADDRESS
.bufferOffset: dw 0
.address: dq 5

vbeInfoBlock:
.signiature: dd 0
.version: dw 0
.oemStringPtr: dd 0
.capabilities: dd 0
.videoModeOffset: dw 0
.videoModeSegment: dw 0
.totalMemory: dw 0
.oemSoftwareRev: dw 0
.oemVendorNamePtr: dd 0
.oemProductNamePtr: dd 0
.oemProductRevPtr: dd 0
.reserved: resb 222
.oemData: resb 256

videoModeInfo:
.attributes: dw 0
.winAAttributes: db 0
.winBAttributes: db 0
.winGranularity: dw 0
.winSize: dw 0
.winASegment: dw 0
.winBSegment: dw 0
.winFuncPtr: dd 0
.bytesPerScanLine: dw 0
.videoWidth: dw 0
.videoHeight: dw 0
.charWidth: db 0
.charHeight: db 0
.numPlanes: db 0
.bitsPerPixel: db 0
.numBanks: db 0
.memoryModel: db 0
.bankSizeKB: db 0
.numImagePages: db 0
.reserved0: db 1
.redMaskSize: db 0
.redFieldPosition: db 0
.greenMaskSize: db 0
.greenFieldPosition: db 0
.blueMaskSize: db 0
.blueFieldPosition: db 0
.reservedMaskSize: db 0
.reservedFieldPosition: db 0
.directColorModeInfo: db 0
.physBasePtr: dd 0
.reserved1: dd 0
.reserved2: dw 0
.linBytesPerScanline: dw 0
.bnkNumImagePages: db 0
.linNmImagePages: db 0
.linRedMaskSize: db 0
.linRedFieldPosition: db 0
.linGreenMaskSize: db 0
.linGreenFieldPosition: db 0
.linBlueMaskSize: db 0
.linBlueFieldPosition: db 0
.linReservedMaskSize: db 0
.linReservedFieldPosition: db 0
.maxPixelClock: dd 0
.reserved3: resb 190 ; TODO: find out why 189 fails???

bits 32

start32:
    mov ax, DATA_SEG
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    mov ss, ax
	
	push dword [videoModeInfo.physBasePtr]
	push word [videoModeInfo.videoWidth]
	push word [videoModeInfo.videoHeight]
	jmp KERNEL_ENTRY_ADDRESS

times (512*4)-($-$$) db 0
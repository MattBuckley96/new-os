bits 16
org 0x7E00

CODE_SEG equ gdtCodeSegment - gdtStart
DATA_SEG equ gdtDataSegment - gdtStart

; TODO: kernel may end up becoming to large to fit under 0x7FFFF
KERNEL_ENTRY_ADDRESS equ 0x10000
VIDEO_WIDTH equ 1600
VIDEO_HEIGHT equ 900
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
	mov ax, [vbeInfoBlock.videoModeSegment]
	mov es, ax
	mov di, [vbeInfoBlock.videoModeOffset]
.next_video_mode:
	mov cx, [es:di]
	add di, 2
	cmp cx, 0xFFFF
	je .no_modes

	; get video mode info
	push es
	xor ax, ax
	mov es, ax
	push di
	mov ax, 0x4F01
	mov di, modeInfoBlock
	int 0x10
	pop di
	pop es
	cmp ax, 0x004F
	jne .error

	; skip if not hardware supported
	mov ax, [modeInfoBlock.attributes]
	and ax, 0x0081
	cmp ax, 0x0081
	jne .next_video_mode

	push word [modeInfoBlock.bitsPerPixel]
	push word [modeInfoBlock.videoHeight]
	push word [modeInfoBlock.videoWidth]
	push cx
	; call debug_print_video_mode_info
	add sp, 8

	mov ax, [modeInfoBlock.videoWidth]
	cmp ax, VIDEO_WIDTH
	jne .next_video_mode
	
	mov ax, [modeInfoBlock.videoHeight]
	cmp ax, VIDEO_HEIGHT
	jne .next_video_mode

	mov al, [modeInfoBlock.bitsPerPixel]
	cmp al, BPP
	jne .next_video_mode

	; found a video mode, now set it
	mov ax, 0x4F02
	mov bx, cx
	or bx, 0x4000 ; make sure its a linear framebuffer
	int 0x10
	cmp ax, 0x004F
	je .done
.no_modes:
	mov si, noVideoModesErrorMessage
	call print
	jmp stop
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
	
	mov ax, 0x2401
	int 0x15
	
	mov al, 0xDD
	out 0x64, al
set_a20:
	jmp .done
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

hexChars: db "0123456789ABCDEF"

print_nibble:
	push si
	mov si, hexChars
	add si, bx

	mov ah, 0x0E
	lodsb
	int 0x10

	pop si
	ret

print_byte:
	push bx
	shr bl, 4
	call print_nibble
	pop bx

	and bl, 0xF
	call print_nibble
	ret

print_bx:
	push ax
	push bx
	shr bx, 8
	call print_byte
	pop bx

	and bx, 0x00FF
	call print_byte

	pop ax
	ret

print_num:
	push si
	mov si, .buffer
	
.loop:
	xor dx, dx
	push cx
	mov cx, 10
	div cx
	pop cx

	push ax
	mov ah, 0x0E
	mov al, dl
	add al, '0'
	mov [si], al
	inc si
	pop ax
	
	cmp ax, 0
    jne .loop

	mov ah, 0x0E
.print_loop:
	dec si
	mov al, [si]
	int 0x10
	cmp si, .buffer
	je .done
	jmp .print_loop
.done:
	pop si
	ret
.buffer: times 10 db 0

debug_print_video_mode_info:
	push bp
	mov bp, sp

	mov bx, [bp + 4]
	call print_bx

	mov ah, 0x0E
	mov al, ':'
	int 0x10
	mov al, ' '
	int 0x10

	mov ax, [bp + 6]
	call print_num
	
	mov ah, 0x0E
	mov al, 'x'
	int 0x10

	mov ax, [bp + 8]
	call print_num

	mov ah, 0x0E
	mov al, 'x'
	int 0x10

	xor ax, ax
	mov al, [bp + 10]
	call print_num

	mov ah, 0x0E
	mov al, 0xA
	int 0x10
	mov al, 0xD
	int 0x10

	pop bp
	ret

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
	jmp $

getVbeInfoErrorMessage: db "Failed to get VBE Info!", 0
getVideoModeInfoErrorMessage: db "Failed to get video mode!", 0
noVideoModesErrorMessage: db "No more modes!", 0
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
.sectorCount: dw 127 ; TODO: dont hardcode this
.bufferOffset: dw 0
.bufferSegment: dw 0x1000
.address: dq 5

align 4
vbeInfoBlock:
.signiature: db "VBE2"
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
.reserved: times 222 db 0
.oemData: times 256 db 0

align 4
modeInfoBlock:
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
.reserved0: db 9
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
.linNumImagePages: db 0
.linRedMaskSize: db 0
.linRedFieldPosition: db 0
.linGreenMaskSize: db 0
.linGreenFieldPosition: db 0
.linBlueMaskSize: db 0
.linBlueFieldPosition: db 0
.linReservedMaskSize: db 0
.linReservedFieldPosition: db 0
.maxPixelClock: dd 0
.reserved3: times 190 db 0

bits 32

start32:
    mov ax, DATA_SEG
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    mov ss, ax
	
	; setup floating point stuff
	mov eax, cr0
	and ax, ~(0b1100) ; clear EM and TS bit
	or ax, 0b100010 ; set MP and NE
	mov cr0, eax
	finit

	push dword [modeInfoBlock.physBasePtr]

	movzx eax, word [modeInfoBlock.videoWidth]
	push eax
	movzx eax, word [modeInfoBlock.videoHeight]
	push eax

	jmp KERNEL_ENTRY_ADDRESS

times (512*4)-($-$$) db 0

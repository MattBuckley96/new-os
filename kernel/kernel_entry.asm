bits 32

section .bss
align 16
stackTop:
	resb 16 * 1024
stackBottom:

section .text
extern kernel_main
global _start:
_start:
	pop ecx
	pop ebx
	pop eax

	mov esp, stackTop
	mov ebp, esp

	; frontbuffer
	push eax ; .memory
	push ecx ; .height
	push ebx ; .width
	call kernel_main

times 510-($-$$) db 0
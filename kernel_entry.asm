bits 32

section .bss
align 16
stackTop:
	resb 16 * 1024
stackBottom:

section .text
extern kernel_main
global _start
_start:
	pop cx
	pop bx
	pop eax

	mov esp, stackTop
	mov ebp, esp

	push ecx
	push ebx
	push eax
	call kernel_main

times 510-($-$$) db 0
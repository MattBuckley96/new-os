bits 32

section .bss
stackTop:
	resb 16 * 1024
stackBottom:

section .text
extern kernel_main
global _start
_start:
	mov esp, stackTop
	mov ebp, esp

	call kernel_main

times 510-($-$$) db 0
global loader
extern main
extern initializeKernelBinary

loader:
	call initializeKernelBinary	; Initialize the kernel binary
	mov rsp, rax				; Set the stack pointer to the kernel's stack
	call main
hang:
	cli
	hlt	; halt machine should kernel return
	jmp hang

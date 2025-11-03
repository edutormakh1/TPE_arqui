
GLOBAL _cli
GLOBAL _sti
GLOBAL picMasterMask
GLOBAL picSlaveMask
GLOBAL haltcpu
GLOBAL _hlt

GLOBAL _irq00Handler
GLOBAL _irq01Handler
GLOBAL _irq02Handler
GLOBAL _irq03Handler
GLOBAL _irq04Handler
GLOBAL _irq05Handler
GLOBAL _irq128Handler

GLOBAL _exception0Handler
GLOBAL _exception6Handler

GLOBAL getPressedKey
GLOBAL reg_array

EXTERN irqDispatcher
EXTERN exceptionDispatcher
EXTERN syscalls
EXTERN printRegisters
EXTERN getStackBase
EXTERN main
EXTERN irqDispatcher
EXTERN exceptionDispatcher

SECTION .text

%macro pushState 0
	
	push rax
	push rbx
	push rcx
	push rdx
	push rbp
	push rdi
	push rsi
	push r8
	push r9
	push r10
	push r11
	push r12
	push r13
	push r14
	push r15
%endmacro

%macro popState 0
		pop r15
	pop r14
	pop r13
	pop r12
	pop r11
	pop r10
	pop r9
	pop r8
	pop rsi
	pop rdi
	pop rbp
	pop rdx
	pop rcx
	pop rbx
	pop rax
	
%endmacro

%macro irqHandlerMaster 1
	pushState

	mov rdi, %1 ; pasaje de parametro
	call irqDispatcher

	; signal pic EOI (End of Interrupt)
	mov al, 20h
	out 20h, al

	popState
	iretq
%endmacro



%macro exceptionHandler 1
	pushState
call printRegisters
	mov rdi, %1 ; primer parámetro: número de excepción
	mov rsi, rsp ; segundo parámetro: puntero al contexto (registros en el stack)
	call exceptionDispatcher


popState ; vuelvo a tener en [rsp] los registros que me pusheo en el stack la interrupción
	call getStackBase	        
	mov qword [rsp+8*3], rax				
	mov qword [rsp], userland	; cambio el valor de rsp en el contexto guardado para que apunte a userland
	iretq
%endmacro

getPressedKey:
	mov rax, [pressed_key]
	ret

_hlt:
	sti
	hlt
	ret

_cli:
	cli
	ret


_sti:
	sti
	ret

picMasterMask:
	push rbp
    mov rbp, rsp
    mov ax, di
    out	21h,al
    pop rbp
    retn

picSlaveMask:
	push    rbp
    mov     rbp, rsp
    mov     ax, di  ; ax = mascara de 16 bits
    out	0A1h,al
    pop     rbp
    retn


;8254 Timer (Timer Tick)
_irq00Handler:
	irqHandlerMaster 0

;Keyboard
_irq01Handler:
pushState

	push rbp 
	mov rbp, rsp
	
	xor rax, rax
	in al, 0x60 ; guardo la tecla
	mov [pressed_key], rax
	cmp rax, SNAPSHOT_KEY
	jne .doNotCapture

	
	mov rax, [rbp+8*1]; r15
	mov [reg_array + 0*8],  rax
	mov rax, [rbp+8*2]; r14
	mov [reg_array + 1*8],  rax
	mov rax, [rbp+8*3]; r13
	mov [reg_array + 2*8],  rax
	mov rax, [rbp+8*4]; r12
	mov [reg_array + 3*8],  rax
	mov rax, [rbp+8*5]; r11
	mov [reg_array + 4*8],  rax
	mov rax, [rbp+8*6]; r10
	mov [reg_array + 5*8],  rax
	mov rax, [rbp+8*7]; r9
	mov [reg_array + 6*8],  rax
	mov rax, [rbp+8*8]; r8
	mov [reg_array + 7*8],  rax
	mov rax, [rbp+8*9]; rsi
	mov [reg_array + 8*8],  rax
	mov rax, [rbp+8*10]; rdi
	mov [reg_array + 9*8], rax
	mov rax, [rbp+8*11]; rbp
	mov [reg_array + 10*8], rax
	mov rax, [rbp+8*12]; rdx
	mov [reg_array + 11*8], rax
	mov rax, [rbp+8*13]; rcx
	mov [reg_array + 12*8], rax
	mov rax, [rbp+8*14]; rbx
	mov [reg_array + 13*8], rax
	mov rax, [rbp+8*15]; rax
	mov [reg_array + 14*8], rax
	mov rax, [rbp+8*16]; rip
	mov [reg_array + 15*8], rax
	mov rax, [rbp+8*17]; cs
	mov [reg_array + 16*8], rax
	mov rax, [rbp+8*18]; rflags
	mov [reg_array + 17*8], rax
	mov rax, [rbp+8*19]; rsp
	mov [reg_array + 18*8], rax
	mov rax, [rbp+8*20]; ss
	mov [reg_array + 19*8], rax
	pop rbp
	popState
	jmp .continue

.doNotCapture:
	pop rbp
	popState

.continue:
	irqHandlerMaster 1

	

;Cascade pic never called
_irq02Handler:
	irqHandlerMaster 2

;Serial Port 2 and 4
_irq03Handler:
	irqHandlerMaster 3

;Serial Port 1 and 3
_irq04Handler:
	irqHandlerMaster 4

;USB
_irq05Handler:
	irqHandlerMaster 5


_irq128Handler:
    pushState
    ; chequear el indice
    cmp rax, 23
    jge .syscall_end
	; rax es el indice, 8 el size de un puntero en 64 bits
    call [syscalls + rax * 8] ; llamamos a la syscall

.syscall_end:
    mov [aux], rax ; preservamos el valor de retorno de la syscall
    popState
    mov rax, [aux]
    iretq

;Zero Division Exception
_exception0Handler:
	exceptionHandler 0


; Invalid Operand Exception
_exception6Handler: 
	exceptionHandler 6

haltcpu:
	cli
	hlt
	ret

SECTION .data 
	userland equ 0x400000 

SECTION .bss
	aux resq 1
	reg_array resq 20 ;20 registros 
	pressed_key resq 1
	SNAPSHOT_KEY equ 0x1D ; Left Ctrl key scancode
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
GLOBAL reg_array ; array donde se almacenan los registros cunado se toco ctrl

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

	mov rdi, %1 ; primer parámetro: número de excepción
	mov rsi, rsp ; segundo parámetro: puntero al contexto (registros en el stack)
	call exceptionDispatcher

	popState
	iretq
%endmacro


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


;Zero Division Exception
_exception0Handler:
	exceptionHandler 0

;Invalid Opcode Exception
_exception6Handler:
	exceptionHandler 6

haltcpu:
	cli
	hlt
	ret

; Syscall handler (int 0x80)
; Espera número de syscall en RAX y argumentos en rdi,rsi,rdx,rcx,r8,r9 (SysV AMD64)
; Llama a la tabla 'syscalls' y devuelve su valor en RAX
_irq128Handler:
	pushState
	; chequear el indice
	cmp rax, 23
	jge .syscall_end
	; Cargar la dirección de la tabla de syscalls de forma RIP-relative y luego indexar
	lea rbx, [rel syscalls]
	call [rbx + rax * 8] ; llamamos a la syscall

.syscall_end:
    mov [aux], rax ; preservamos el valor de retorno de la syscall
    popState
    mov rax, [aux]
    iretq

SECTION .data 
	userland equ 0x400000 

SECTION .bss
	aux resq 1
	pressed_key resq 1
	SNAPSHOT_KEY equ 0x1D ; Left Ctrl key scancode

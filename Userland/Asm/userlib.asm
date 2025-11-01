global sys_write, sys_read, sys_date, sys_time, sys_regs, sys_clear
global sys_increase_fontsize, sys_decrease_fontsize, sys_beep
global sys_screen_size, sys_circle, sys_rectangle, sys_line, sys_draw_string
global sys_enable_textmode, sys_disable_textmode, sys_put_pixel, sys_key_status
global sys_sleep, sys_clear_input_buffer, sys_ticks,sys_speaker_start,sys_speaker_stop
global generate_invalid_opcode
global printf
global scanf
extern printf_aux
extern scanf_aux

; Macro para simplificar las syscalls
%macro SYSCALL 1
    mov rax, %1
    int 0x80
    ret
%endmacro


; 0 – void regs(char* buf);
sys_regs:
    SYSCALL 0

; 1 – void time(uint8_t* buf);
sys_time:
    SYSCALL 1

; 2 – void date(uint8_t* buf);
sys_date:
    SYSCALL 2

; 3 – uint64_t read(char* buf, uint64_t count);
sys_read:
    SYSCALL 3

; 4 – uint64_t write(uint64_t fd, const char* buf, uint64_t count);
sys_write:
    SYSCALL 4

; 5 – void increase_fontsize(void);
sys_increase_fontsize:
    SYSCALL 5

; 6 – void decrease_fontsize(void);
sys_decrease_fontsize:
    SYSCALL 6

; 7 – void beep(uint32_t freq, uint64_t dur);
sys_beep:
    SYSCALL 7

; 8 – void screen_size(uint32_t* w, uint32_t* h);
sys_screen_size:
    SYSCALL 8

; 9 – void circle(uint64_t fill, uint64_t* info, uint32_t color);
sys_circle:
    SYSCALL 9

; 10 – void rectangle(uint64_t fill, uint64_t* info, uint32_t color);
sys_rectangle:
    SYSCALL 10

; 11 – void line(uint64_t* info, uint32_t color);
sys_line:
    SYSCALL 11

; 12 – void draw_string(const char* s, uint64_t* info, uint32_t color);
sys_draw_string:
    SYSCALL 12

; 13 – void clear(void);
sys_clear:
    SYSCALL 13

; 14 - void speaker_start(uint32_t freq_hz);
sys_speaker_start:
    SYSCALL 14

; 15 - void speaker_stop();
sys_speaker_stop:
    SYSCALL 15

; 16 - void sys_textmode()
sys_enable_textmode:
    SYSCALL 16

; 17 - void sys_videomode()
sys_disable_textmode:
    SYSCALL 17

; 18 - static void sys_putpixel(uint32_t hexColor, uint64_t x, uint64_t y)
sys_put_pixel:
    SYSCALL 18

; 19 – uint64_t key_status(char);
sys_key_status:
    SYSCALL 19

; 20 - void sys_sleep(uint64_t miliseconds)
sys_sleep:
    SYSCALL 20

; 21 - void sys_clear_input_buffer()
sys_clear_input_buffer:
    SYSCALL 21

; 22 - uint64_t sys_ticks()
sys_ticks:
    SYSCALL 22

; Generar excepción de opcode inválido
generate_invalid_opcode:
    ud2
    ret

; Implementación de printf
printf:
    push rbp
    mov rbp, rsp

    ; Crear espacio para los argumentos en la pila
    sub rsp, 104               ; 13 × 8 bytes (5 enteros y 8 flotantes)
    mov [rsp+8*0], rsi         ; args[0] = segundo argumento
    mov [rsp+8*1], rdx
    mov [rsp+8*2], rcx
    mov [rsp+8*3], r8
    mov [rsp+8*4], r9

    ; Guardar argumentos flotantes
    movsd [rsp+8*5], xmm0
    movsd [rsp+8*6], xmm1
    movsd [rsp+8*7], xmm2
    movsd [rsp+8*8], xmm3
    movsd [rsp+8*9], xmm4
    movsd [rsp+8*10], xmm5
    movsd [rsp+8*11], xmm6
    movsd [rsp+8*12], xmm7

    ; Preparar argumentos para printf_aux
    lea rsi, [rsp]             ; args[0]
    lea rdx, [rbp+16]          ; stackPtr
    lea rcx, [rsp+8*5]         ; floatArgs
    call printf_aux

    leave
    ret

; Implementación de scanf
scanf:
    push rbp
    mov rbp, rsp

    ; Crear espacio para los argumentos en la pila
    sub rsp, 40                ; 5 × 8 bytes
    mov [rsp+8*0], rsi         ; args[0] = segundo argumento
    mov [rsp+8*1], rdx
    mov [rsp+8*2], rcx
    mov [rsp+8*3], r8
    mov [rsp+8*4], r9

    ; Preparar argumentos para scanf_aux
    lea rsi, [rsp]             ; args[0]
    lea rdx, [rbp+16]          ; stackPtr
    call scanf_aux

    leave
    ret
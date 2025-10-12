#ifndef USERLIB_H
#define USERLIB_H

#include <stdint.h>

//file descriptors
#define STDOUT 1
#define STDERR 2

#define FLOAT_PRECISION 6

//tamaños de buffers
#define REGSBUF_SIZE 800
#define TIMEBUF_SIZE 3 

//cantidad de registros para argumentos de syscalls
#define NUM_INT_REGS 5 //registros enteros (rbx,rcx,rdx,rsi,rdi)
#define NUM_SSE_REGS 8 // SSE (floats y doubles)

// Buffers para conversiones numéricas
#define BINARY_BUFFER_SIZE  65  // 64 bits + terminador nulo
#define OCTAL_BUFFER_SIZE   23  // 22 dígitos + terminador nulo
#define DECIMAL_BUFFER_SIZE 21  // 20 dígitos + terminador nulo
#define HEX_BUFFER_SIZE     17  // 16 dígitos + terminador nulo

// SYSTEMCALLS DIRECTO DE ASM // a medida que codeamos seguro hay que ir agregando mas
extern uint64_t sys_regs(char *buf);
extern void     sys_time(uint8_t *buf);     
extern void     sys_date(uint8_t *buf);
extern uint64_t sys_read(char *buf,  uint64_t count);
extern uint64_t sys_write(uint64_t fd, const char *buf, uint64_t count);
extern void     sys_increase_fontsize();
extern void     sys_decrease_fontsize();
extern void     sys_beep(uint32_t freq_hz, uint64_t duration_ms);
extern void     sys_screen_size(uint32_t *width, uint32_t *height);
extern void     sys_clear(void);
extern void     sys_speaker_start(uint32_t freq_hz);
extern void     sys_speaker_stop(void);
extern void 	sys_enable_textmode();
extern void 	sys_disable_textmode();
extern void     sys_put_pixel(uint32_t color, uint64_t x, uint64_t y);
extern uint64_t sys_key_status(char key);
extern void     sys_sleep(uint64_t miliseconds);
extern void     sys_clear_input_buffer();
extern uint64_t sys_ticks();
extern void generate_invalid_opcode();
extern uint64_t printf(const char *fmt, ...);
extern uint64_t scanf(const char *fmt, ...);

// FUNCIONES DE I/O 
uint64_t print_err(char *str);                    
uint64_t print_string(char *str);                
uint64_t putchar(char c);                         
char getchar(void);                               
char getchar_nonblock();                          
uint64_t printf_aux(const char *fmt, const uint64_t *regArgs, const uint64_t *stackPtr, const double *floatArgs);            
uint64_t get_key_status(char key); 

// FUNCIONES DE STRINGS 
uint64_t strlen(char * str);
int strcmp(char * s1, char * s2);
uint64_t num_to_str(uint64_t value, char * buffer, uint32_t base);

#endif
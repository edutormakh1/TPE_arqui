#ifndef _SYSCALL_DISPATCHER_H_
#define _SYSCALL_DISPATCHER_H_

#include <stdint.h>

#define STDOUT 1
#define STDERR 2

#define SYSCALL_COUNT 23 // incrementar al agregar una syscall y cambiar el chequeo del indice en asm

extern void * syscalls[SYSCALL_COUNT];

// pueden recibir hasta 3 argumentos


static void sys_rectangle(uint64_t fill, uint64_t * info, uint32_t color);
static void sys_draw_line(uint64_t * info, uint32_t color);
static void sys_draw_string(const char * buf, uint64_t * info, uint32_t color);



#endif
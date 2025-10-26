#ifndef EXCEPTIONS_H
#define EXCEPTIONS_H

#include <stdint.h>

// Estructura que contiene el estado de todos los registros del procesador
// cuando ocurre una excepción
typedef struct {
    // Registros guardados por pushState (en orden inverso al push)
    uint64_t r15;
    uint64_t r14;
    uint64_t r13;
    uint64_t r12;
    uint64_t r11;
    uint64_t r10;
    uint64_t r9;
    uint64_t r8;
    uint64_t rsi;
    uint64_t rdi;
    uint64_t rbp;
    uint64_t rdx;
    uint64_t rcx;
    uint64_t rbx;
    uint64_t rax;
    
    // Registros guardados automáticamente por el CPU al entrar en la excepción
    uint64_t rip;    // Instruction Pointer
    uint64_t cs;     // Code Segment
    uint64_t rflags; // Flags
    uint64_t rsp;    // Stack Pointer
    uint64_t ss;     // Stack Segment
} RegisterContext;

// Tipo de puntero a función manejadora de excepciones
typedef void (*Exception)(RegisterContext *ctx);

// Función principal que despacha las excepciones
void exceptionDispatcher(int exception, RegisterContext *ctx);

#endif // EXCEPTIONS_H


#ifndef EXCEPTIONS_H
#define EXCEPTIONS_H




// Tipo de puntero a función manejadora de excepciones
typedef void (*Exception)(void);

// Función principal que despacha las excepciones
void exceptionDispatcher(int exception);

static void excepHandler(char * msg);
static void zero_division();
static void invalid_opcode();

#endif 


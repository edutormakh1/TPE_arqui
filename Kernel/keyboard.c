#include "keyboard.h"
#include "lib.h"
#include "naiveConsole.h"
#include "video_driver.h"
#include <stdint.h>

static int shift = 0 ;
static int capsLock = 0;
static int copied_registers=0;

uint16_t buffer_start = 0; // índice del buffer del próximo carácter a leer 
uint16_t buffer_end = 0; // índice del buffer donde se va a escribir el próximo caracter recibido en el teclado
uint16_t buffer_current_size = 0; // cantidad de caracteres en el buffer actual (listos para ser leídos)

static uint8_t buffer[BUFFER_LENGTH];
static char reg_buff[800]; // ACA FIJARNOS QUÉ TAMAÑO NOS CONVIENE

static void writeBuffer(unsigned char c);

static const char lowerKeys[] = {
      0,   27, '1', '2', '3', '4', '5', '6', '7', '8', '9',  '0', '-', '=',
   '\b', '\t', 'q', 'w', 'e', 'r', 't', 'y', 'u', 'i', 'o', 'p',  '[', ']',
   '\n',    0, 'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', ';', '\'', '`',
      0, '\\', 'z', 'x', 'c', 'v', 'b', 'n', 'm', ',', '.', '/',    0, '*',
      0, ' ',   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,    0,   0,
      0,    0,   38,   0, '-',   37,   0,   39, '+',   0,   40,   0,    0,   0,
      0,    0,   0,   0,   0,   0,   0,   0,  0,    0,   0,   0,    0,   0,
};

static const char upperKeys[] = {
      0,   27, '!', '@', '#', '$', '%', '^', '&', '*', '(', ')', '_', '+',
   '\b', '\t', 'Q', 'W', 'E', 'R', 'T', 'Y', 'U', 'I', 'O', 'P', '{', '}',
   '\n',    0, 'A', 'S', 'D', 'F', 'G', 'H', 'J', 'K', 'L', ':', '"', '~',
      0, '|', 'Z', 'X', 'C', 'V', 'B', 'N', 'M', '<', '>', '?',   0, '*',
      0, ' ',    0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
      0,   0,    0,   0, '-',   0,   0,   0, '+',   0,   0,   0,   0,   0,
      0,   0,    0,   0,   0,   0
};

static const char * scancodeToAscii[] = {lowerKeys, upperKeys};

static uint8_t pressedKeys[LETTERS] = {0};

// Static porque no queremos que se pueda acceder desde otro archivo
static void writeBuffer(unsigned char c) {
    buffer[buffer_end] = c;
    buffer_end = (buffer_end + 1) % BUFFER_LENGTH; // si hay buffer overflow, se pisa lo del principio (jodete usuario)
    buffer_current_size = (buffer_current_size + 1) % BUFFER_LENGTH;
}

void clear_buffer() {
  buffer_end = buffer_start = buffer_current_size = 0;
}

uint8_t getCharFromBuffer() {
    if(buffer_current_size == 0){
        return (uint8_t)-1;
    }
    --buffer_current_size;
    uint8_t result = buffer[buffer_start];
    buffer_start = (buffer_start + 1) % BUFFER_LENGTH;
    return result;
}

// copia en el buff lo que hay en el buffer de teclado hasta count y va vaciando el buffer de teclado
uint64_t read_keyboard_buffer (char * buff_copy, uint64_t count) {
    int i;
    for (i=0; i < count && i < buffer_current_size; i++) {
        buff_copy[i] = getCharFromBuffer();
    }
    return i;
}


void handlePressedKey() {
    uint8_t scancode = getPressedKey(); // this function retrieves the pressed key's scancode

    if (scancode == LEFT_SHIFT || scancode == RIGHT_SHIFT){ 
        shift = 1;
    } else if (scancode == LEFT_SHIFT + BREAKCODE_OFFSET || scancode == RIGHT_SHIFT + BREAKCODE_OFFSET) { 
        shift = 0;
    } else if (scancode == LEFT_CONTROL) {
        // mark that user requested register snapshot; actual snapshot implementation is optional
        copied_registers = 1;
        return; 
    } else if (scancode == CAPS_LOCK) {
        capsLock = (capsLock+1)%2;
    } else if (scancode == 0){
        return;
    }else if (scancode > BREAKCODE_OFFSET){ // se soltó una tecla o es un caracter no imprimible
        char raw = lowerKeys[scancode - BREAKCODE_OFFSET]; 
        if (raw >= 'a' && raw <= 'z') {
            pressedKeys[raw-'a'] = 0; // marcamos la tecla como no presionada
        }
        return;
    } else if (scancode == UP_ARROW || scancode == DOWN_ARROW || scancode == LEFT_ARROW || scancode == RIGHT_ARROW || scancode==0){  
        return;
    } else {
        int index;                      
        char raw = lowerKeys[scancode]; 
        int isLetter = (raw >= 'a' && raw <= 'z');  

        if (isLetter) {
            index = shift ^ capsLock;
            pressedKeys[raw-'a'] = 1;
        } else {
            index = shift;                      
        }

        writeBuffer(scancodeToAscii[index][scancode]);
        // vdPutChar(scancodeToAscii[index][scancode], 0xffffff);
    }

    return;
}

void writeStringToBuffer(const char *str) {
    while (*str) {
        writeBuffer((unsigned char)*str);
        str++;
    }
}

uint8_t isPressedKey(char c) {
    if ((c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z')) { 
        c = (c >= 'A' && c <= 'Z') ? c - 'A' + 'a' : c; // Convertir a minúscula si es mayúscula
        return pressedKeys[c-'a']; // Devuelve 1 si la tecla está presionada, 0 si no
    }
    return 0; // Si el char es inválido, retornamos 0
}


// Blocking line read that echoes to the text-mode console (naiveConsole).
// Implements basic editing: backspace and enter. The buffer is null-terminated.
void readLine(char *buffer, unsigned long maxLen) {
    unsigned long idx = 0;
    uint8_t c;

    if (maxLen == 0) return;

    while (1) {
        // Wait for a character in the keyboard ring buffer
        c = getCharFromBuffer();
        if (c == (uint8_t)-1) {
            continue; // busy-wait; interrupts should fill the buffer
        }

        // Handle newline / carriage return -> finish
        if (c == '\r' || c == '\n') {
            buffer[idx] = '\0';
            ncNewline();
            return;
        }

        // Handle backspace
        if (c == '\b') {
            if (idx > 0) {
                idx--;
                ncPrintChar('\b');
            }
            continue;
        }

        // Printable character
        if (idx < maxLen - 1) {
            buffer[idx++] = (char)c;
            ncPrintChar((char)c);
        } else {
            // buffer full: ignore additional characters
        }
    }
}





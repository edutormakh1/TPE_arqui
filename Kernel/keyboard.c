#include "include/keyboard.h"
#include "include/lib.h"
#include "include/naiveConsole.h"
#include "include/video_driver.h"
#include <stdint.h>

static int shift = 0 ;
static int capsLock = 0;
static int copied_registers=0;

uint16_t buffer_start = 0; // índice del buffer del próximo carácter a leer 
uint16_t buffer_end = 0; // índice del buffer donde se va a escribir el próximo caracter recibido en el teclado
uint16_t buffer_current_size = 0; // cantidad de caracteres en el buffer actual (listos para ser leídos)

static uint8_t buffer[BUFFER_LENGTH];
// static char reg_buff[800]; // ACA FIJARNOS QUÉ TAMAÑO NOS CONVIENE (no usado)

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



// Lee una tecla y la dibuja en modo gráfico VBE (usa scancodeToAscii con shift/caps)
char readKeyAsciiBlockingVBE(uint32_t *x, uint32_t y, uint32_t color) {
    while (1) {
        uint8_t sc = (uint8_t)getPressedKey();
        if (sc >= BREAKCODE_OFFSET) {
            // ignorar break codes
            continue;
        }
        char raw = lowerKeys[sc];
        int isLetter = (raw >= 'a' && raw <= 'z');
        int index = isLetter ? (shift ^ capsLock) : shift;
        char ascii = scancodeToAscii[index][sc];
        if (ascii != 0) {
            if ((unsigned char)ascii >= 32) {
                drawChar(*x, y, ascii, color, 3);
                
                *x += 8;
            }
            return ascii;
        }
    }
}

// Lee una línea de texto en modo gráfico VBE y la deja en buffer (actualiza x mientras escribe)
void readLineVBE(char *buffer, unsigned long maxLen, uint32_t *x, uint32_t y, uint32_t color) {
    if (maxLen == 0) return;
    unsigned long pos = 0;
    buffer[0] = 0;
    for (;;) {
        char c = readKeyAsciiBlockingVBE(x, y, color);
        if (c == 0) continue;
        if (c == '\n' || c == '\r') {
            buffer[pos] = 0;
            return;
        }
        if (c == '\b') {
            if (pos > 0) {
                pos--;
                buffer[pos] = 0;
                // mover cursor hacia atrás y borrar el último glyph
                if (*x >= 8) { *x -= 8; }
                drawFilledRect(*x, y, 8, 16, 0x00000000);
            }
            continue;
        }
        if ((unsigned char)c < 32) continue;
        if (pos < maxLen - 1) {
            buffer[pos++] = c;
            buffer[pos] = 0;
        }
    }
}




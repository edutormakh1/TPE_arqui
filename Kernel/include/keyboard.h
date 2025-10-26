#ifndef KEYBOARD_H
#define KEYBOARD_H

#include <stdint.h>

void printPressedKey();

// Returns an ASCII character for the next key press (blocks until a valid ASCII is available).
// Ignores break codes and extended prefixes. Returns 0 for non-printables.
char readKeyAsciiBlocking();

// Reads a line from keyboard into buffer, echoing to screen using naiveConsole.
// - buffer: destination buffer
// - maxLen: maximum characters including terminating null
// Editing: supports backspace; ends on Enter (\n or \r). Buffer is null-terminated.
void readLine(char *buffer, unsigned long maxLen);

// Note: VBE-specific readLine/readKey functions removed to keep compatibility with existing kernel.

// Buffer sizes and scancode constants
#define BUFFER_LENGTH 1024
#define BREAKCODE_OFFSET 128
#define LEFT_SHIFT 42
#define RIGHT_SHIFT 54
#define LEFT_CONTROL 29
#define CAPS_LOCK 58
#define UP_ARROW 72
#define DOWN_ARROW 80
#define LEFT_ARROW 75
#define RIGHT_ARROW 77
#define LETTERS 26

// Ring-buffer keyboard API
void clear_buffer(void);
uint8_t getCharFromBuffer(void);
uint64_t read_keyboard_buffer(char * buff_copy, uint64_t count);
void writeStringToBuffer(const char *str);
uint8_t isPressedKey(char c);
// ISR keyboard handler
void handlePressedKey(void);

// Entrada de texto con eco gráfico en modo VBE
void input_line(char *buffer, unsigned int maxLen, uint32_t x, uint32_t y, uint32_t color);
char readKeyAsciiBlockingVBE(uint32_t *x, uint32_t y, uint32_t color);
void readLineVBE(char *buffer, unsigned long maxLen, uint32_t *x, uint32_t y, uint32_t color);

#endif
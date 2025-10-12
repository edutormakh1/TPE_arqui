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



#endif
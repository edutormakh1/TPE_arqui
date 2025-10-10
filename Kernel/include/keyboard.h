#ifndef KEYBOARD_H
#define KEYBOARD_H

void printPressedKey();

// Returns an ASCII character for the next key press (blocks until a valid ASCII is available).
// Ignores break codes and extended prefixes. Returns 0 for non-printables.
char readKeyAsciiBlocking();

#endif
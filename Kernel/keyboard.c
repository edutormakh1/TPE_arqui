#include "keyboard.h"
#include "lib.h"
#include "naiveConsole.h"

char scancode_to_ascii[128] = {
    0,  27, '1','2','3','4','5','6','7','8','9','0','-','=', '\b', // backspace
    '\t','q','w','e','r','t','y','u','i','o','p','[',']','\n',     // enter
    0,'a','s','d','f','g','h','j','k','l',';','\'','`',
    0,'\\','z','x','c','v','b','n','m',',','.','/', 0, '*',
    0,' ', // space bar
    // ... (resto omitido)
};

void printPressedKey() {
    while (1) {
        uint8_t scancode = getPressedKey();
        if (scancode < 128) {
            ncPrintChar(scancode_to_ascii[scancode]);
        }   
    }
}

// Block until we get a make scancode (< 0x80) and translate to ASCII.
// Returns 0 for non-printable keys or if scancode has no mapping.
char readKeyAsciiBlocking() {
    while (1) {
        uint8_t sc = getPressedKey();
        // Handle break codes (key release)
        if (sc & 0x80) {
            continue; // ignore releases
        }
        // Ignore extended scancode prefixes (0xE0, 0xE1) if they arrive
        if (sc == 0xE0 || sc == 0xE1) {
            continue;
        }
        if (sc < sizeof(scancode_to_ascii)) {
            char ch = scancode_to_ascii[sc];
            return ch; // may be 0 for non-printables; caller can handle
        }
        // If out of range, keep waiting
    }
}
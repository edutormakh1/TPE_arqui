#include "keyboard.h"
#include "lib.h"
#include "naiveConsole.h"
#include "video_driver.h"
#include <stdint.h>

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

char readKeyAsciiBlocking(); // forward

void readLine(char *buffer, unsigned long maxLen) {
    if (maxLen == 0) return;
    unsigned long pos = 0;
    buffer[0] = 0;
    for(;;) {
        char c = readKeyAsciiBlocking();
        if (c == 0) continue;
        if (c == '\n' || c == '\r') {
            ncNewline();
            buffer[pos] = 0;
            return;
        }
        if (c == '\b') {
            if (pos > 0) {
                pos--;
                buffer[pos] = 0;
                // erase last char visually
                ncPrintChar('\b');
                ncPrintChar(' ');
                ncPrintChar('\b');
            }
            continue;
        }
        if ((unsigned char)c < 32) {
            continue; // ignore other control chars
        }
        if (pos < maxLen - 1) {
            buffer[pos++] = c;
            buffer[pos] = 0;
            ncPrintChar(c); // echo
        }
    }
}

char readKeyAsciiBlocking() {
    // Block until we get a make scancode (< 128) that maps to a non-zero ASCII
    while (1) {
        uint8_t sc = (uint8_t)getPressedKey();
        if (sc >= 128) {
            // break code or extended, ignore
            continue;
        }
        char ascii = scancode_to_ascii[sc];
        // Return even if ascii is special (\b, \n, tab, space). If 0, keep waiting
        if (ascii != 0) {
            return ascii;
        }
    }
}

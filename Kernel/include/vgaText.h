#ifndef VGA_TEXT_H
#define VGA_TEXT_H

#include <stdint.h>

// Draw a single ASCII character at pixel position (x, y) using an 8x8 font
// color: palette index (0..255)
void vga_draw_char(uint16_t x, uint16_t y, char c, uint8_t color);

// Draw a null-terminated string starting at (x, y), advancing 8 px per char
void vga_draw_string(uint16_t x, uint16_t y, const char *s, uint8_t color);

// Simple input line in graphics mode: echoes printable ASCII as you type,
// supports backspace, finishes on Enter, writes into buffer (null-terminated).
// max_len includes the terminating null.
void vga_text_input(uint16_t x, uint16_t y, char *buffer, uint16_t max_len, uint8_t color);

#endif // VGA_TEXT_H

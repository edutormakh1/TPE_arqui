#ifndef VIDEO_DRIVER_H
#define VIDEO_DRIVER_H
#include <stdint.h>

void putPixel(uint32_t x, uint32_t y, uint32_t color);
void drawLine(uint32_t x0, uint32_t y0, uint32_t x1, uint32_t y1, uint32_t color);

// Draw a single 8x16 glyph for ASCII character 'c' at (x, y)
void drawGlyph8x16(uint32_t x, uint32_t y, char c, uint32_t color);

// Draw a null-terminated string starting at (x, y)
void drawString8x16(uint32_t x, uint32_t y, const char *s, uint32_t color);
void gfxNewline(int *cursorY);

#endif // VIDEO_DRIVER_H

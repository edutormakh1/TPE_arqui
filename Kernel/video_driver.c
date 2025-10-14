#include "video_driver.h"
#include <stdint.h>
#include "include/font.h"

#define VBEModeInfoBlock ((uint8_t*)0x0000000000005C00)

static inline int kabs(int x) { return x < 0 ? -x : x; }

static inline uint32_t getPhysBasePtr() {
    return *(uint32_t*)(VBEModeInfoBlock + 40);
}
static inline uint16_t getXResolution() {
    return *(uint16_t*)(VBEModeInfoBlock + 18);
}
static inline uint8_t getBitsPerPixel() {
    return *(uint8_t*)(VBEModeInfoBlock + 25);
}

void putPixel(uint32_t x, uint32_t y, uint32_t color) {
    uint32_t physBase = getPhysBasePtr();
    uint16_t width = getXResolution();
    uint8_t bpp = getBitsPerPixel();
    uint32_t offset = (y * width + x) * (bpp / 8);
    volatile uint8_t* fb = (volatile uint8_t*)physBase;
    fb[offset + 0] = (color & 0xFF);        // Blue
    fb[offset + 1] = ((color >> 8) & 0xFF); // Green
    fb[offset + 2] = ((color >> 16) & 0xFF);// Red
    if (bpp == 32)
        fb[offset + 3] = ((color >> 24) & 0xFF); // Alpha (opcional)
}

// Bresenham's line algorithm
void drawLine(uint32_t x0, uint32_t y0, uint32_t x1, uint32_t y1, uint32_t color) {
    int dx = kabs((int)x1 - (int)x0), sx = x0 < x1 ? 1 : -1;
    int dy = -kabs((int)y1 - (int)y0), sy = y0 < y1 ? 1 : -1;
    int err = dx + dy, e2;
    while (1) {
        putPixel(x0, y0, color);
        if (x0 == x1 && y0 == y1) break;
        e2 = 2 * err;
        if (e2 >= dy) { err += dy; x0 += sx; }
        if (e2 <= dx) { err += dx; y0 += sy; }
    }
}

void drawFilledRect(uint32_t x, uint32_t y, uint32_t width, uint32_t height, uint32_t color) {
    for (uint32_t i = 0; i < height; i++) {
        for (uint32_t j = 0; j < width; j++) {
            putPixel(x + j, y + i, color);
        }
    }
}

void drawGlyph8x16(uint32_t x, uint32_t y, char c, uint32_t color) {
    uint8_t uc = (uint8_t)c;
    const uint8_t *glyph = font[uc]; // Usar la fuente de font.h
    for (uint32_t row = 0; row < FONT_HEIGHT; row++) {
        uint8_t bits = glyph[row];
        for (uint32_t col = 0; col < FONT_WIDTH; col++) {
            if (bits & (0x80 >> col)) {
                putPixel(x + col, y + row, color);
            }
        }
    }
}

void drawString8x16(uint32_t x, uint32_t y, const char *s, uint32_t color) {
    uint32_t cx = x;
    for (const char *p = s; *p; ++p) {
        if (*p == '\n') {
            y += 16;
            cx = x;
            continue;
        }
        drawGlyph8x16(cx, y, *p, color);
        cx += 8;
    }
}
void gfxNewline(int *cursorY) {
    *cursorY += 16; // Avanza una línea (ajusta si tu fuente tiene otra altura)
}

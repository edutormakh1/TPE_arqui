#include "video_driver.h"
#include <stdint.h>
#include "font.h"

struct vbe_mode_info_structure {
    uint16_t attributes;
    uint8_t window_a; //ventana principal para lectura/escritura
    uint8_t window_b; //ventana secundaria para leer o doble buffer
    uint16_t granularity; //tamano min de desplazamiento en Kbytes entre 2 ventanas
    uint16_t window_size; //tamano de c/d ventana en Kbytes
    uint16_t segment_a; 
    uint16_t segment_b;
    uint32_t win_func_ptr; //puntero a una func de la BIOS que permite cambiar
    //que parte de la memoria se muestra en la ventana 
    uint16_t pitch; //cant bytes por fila de pantalla
    uint16_t width; //ancho de pixeles
    uint16_t height; //alto en pixeles
    uint8_t w_char;
    uint8_t y_char;
    uint8_t planes;
    uint8_t bpp; 
    uint8_t banks;
    uint8_t memory_model;
    uint8_t bank_size;
    uint8_t image_pages;
    uint8_t reserved0;
    uint8_t red_mask; //campos de color de aca para adelante
    uint8_t red_position;
    uint8_t green_mask;
    uint8_t green_position;
    uint8_t blue_mask;
    uint8_t blue_position;
    uint8_t reserved_mask;
    uint8_t reserved_position;
    uint8_t direct_color_attributes;
    uint32_t framebuffer;
    uint32_t off_screen_mem_off;
    uint16_t off_screen_mem_size;
    uint8_t reserved1[206];
} __attribute__ ((packed));

typedef struct vbe_mode_info_structure * VBEInfoPtr;
VBEInfoPtr VBE_mode_info = (VBEInfoPtr) 0x0000000000005C00;

static inline int kabs(int x) { return x < 0 ? -x : x; }

void putPixel(uint32_t hexColor, uint64_t x, uint64_t y) {
    uint8_t * framebuffer = (uint8_t *) VBE_mode_info->framebuffer;
    uint64_t offset = (x * ((VBE_mode_info->bpp)/8)) + (y * VBE_mode_info->pitch); //calcula posicion del pixel dentro de la memoria
    framebuffer[offset]     =  (hexColor) & 0xFF;
    framebuffer[offset+1]   =  (hexColor >> 8) & 0xFF; 
    framebuffer[offset+2]   =  (hexColor >> 16) & 0xFF;
}

void drawLine(uint32_t x0, uint32_t y0, uint32_t x1, uint32_t y1, uint32_t color) {
    int dx = kabs((int)x1 - (int)x0), sx = x0 < x1 ? 1 : -1;
    int dy = -kabs((int)y1 - (int)y0), sy = y0 < y1 ? 1 : -1;
    int err = dx + dy, e2;
    while (1) {
        putPixel(color, x0, y0);
        if (x0 == x1 && y0 == y1) break;
        e2 = 2 * err;
        if (e2 >= dy) { err += dy; x0 += sx; }
        if (e2 <= dx) { err += dx; y0 += sy; }
    }
}

void drawFilledRect(uint32_t x, uint32_t y, uint32_t width, uint32_t height, uint32_t color) {
    for (uint32_t i = 0; i < height; i++) {
        for (uint32_t j = 0; j < width; j++) {
            putPixel(color, x + j, y + i);
        }
    }
}

void drawGlyph8x16(uint32_t x, uint32_t y, char c, uint32_t color) {
    uint8_t uc = (uint8_t)c;
    const uint8_t *glyph = font[uc];
    for (uint32_t row = 0; row < FONT_HEIGHT; row++) {
        uint8_t bits = glyph[row];
        for (uint32_t col = 0; col < FONT_WIDTH; col++) {
            if (bits & (0x80 >> col)) {
                putPixel(color, x + col, y + row);
            }
        }
    }
}

void drawString8x16(uint32_t x, uint32_t y, const char *s, uint32_t color) {
    uint32_t cx = x;
    for (const char *p = s; *p; ++p) {
        if (*p == '\n') {
            y += FONT_HEIGHT;
            cx = x;
            continue;
        }
        drawGlyph8x16(cx, y, *p, color);
        cx += FONT_WIDTH;
    }
}

void gfxNewline(int *cursorY) {
    *cursorY += FONT_HEIGHT;
}

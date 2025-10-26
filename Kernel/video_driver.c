#include "video_driver.h"
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include <string.h>
#include "font.h"

#define ENABLED 1
#define DISABLED 0
#define BKG_COLOR 0x000000
#define MAX_SIZE 4

// Estructura VBE (packed)
struct vbe_mode_info_structure {
    uint16_t attributes;
    uint8_t window_a;
    uint8_t window_b;
    uint16_t granularity;
    uint16_t window_size;
    uint16_t segment_a;
    uint16_t segment_b;
    uint32_t win_func_ptr;
    uint16_t pitch; // bytes por fila
    uint16_t width; // ancho en pixeles
    uint16_t height; // alto en pixeles
    uint8_t w_char;
    uint8_t y_char;
    uint8_t planes;
    uint8_t bpp;
    uint8_t banks;
    uint8_t memory_model;
    uint8_t bank_size;
    uint8_t image_pages;
    uint8_t reserved0;
    uint8_t red_mask;
    uint8_t red_position;
    uint8_t green_mask;
    uint8_t green_position;
    uint8_t blue_mask;
    uint8_t blue_position;
    uint8_t reserved_mask;
    uint8_t reserved_position;
    uint8_t direct_color_attributes;
    uint32_t framebuffer; // direccion de memoria de video (linear)
    uint32_t off_screen_mem_off;
    uint16_t off_screen_mem_size;
    uint8_t reserved1[206];
} __attribute__ ((packed));

typedef struct vbe_mode_info_structure * VBEInfoPtr;
static VBEInfoPtr VBE_mode_info = (VBEInfoPtr)(uintptr_t)0x5C00; // 0x5C00

static inline int kabs(int x) { return x < 0 ? -x : x; }

static bool isValid(uint64_t x, uint64_t y) {
    return x < VBE_mode_info->width && y < VBE_mode_info->height;
}

void putPixel(uint32_t hexColor, uint64_t x, uint64_t y) {
    if (!isValid(x, y)) return;

    uint8_t *framebuffer = (uint8_t *)(uintptr_t)VBE_mode_info->framebuffer;
    uint32_t bpp = VBE_mode_info->bpp;
    uint32_t bytes_per_pixel = (bpp + 7) / 8; // redondeo hacia arriba por si hay 24 o 32
    uint64_t offset = y * (uint64_t)VBE_mode_info->pitch + x * bytes_per_pixel;

    // Escribir cada byte del color (little-endian esperable)
    for (uint32_t i = 0; i < bytes_per_pixel; i++) {
        framebuffer[offset + i] = (hexColor >> (8 * i)) & 0xFF;
    }
}

uint16_t getScreenHeight(void) {
    return VBE_mode_info->height;
}

uint16_t getScreenWidth(void) {
    return VBE_mode_info->width;
}

/* MODO TEXTO */
static int text_mode = DISABLED;
static uint64_t cursor_x;
static uint32_t cursor_y;
static uint8_t text_size = 1;

void enableTextMode(void) {
    if (text_mode) return;
    text_mode = ENABLED;
    vdClear();
}

void disableTextMode(void) {
    if (!text_mode) return;
    vdClear();
    text_mode = DISABLED;
}

void vdSetTextSize(uint8_t size) {
    if (size == 0) size = 1;
    text_size = size;
}

uint8_t vdGetTextSize(void) {
    return text_size;
}

// Desplaza todo el contenido de la pantalla hacia arriba una línea
static void scrollUp() {
    uint64_t line_height = text_size * FONT_HEIGHT;
    uint8_t * framebuffer = (uint8_t *) VBE_mode_info->framebuffer;
    
    // Usar memcpy para copiar cada línea completa
    // desde la segunda línea de texto hasta el final hacia arriba
    for (uint64_t src_y = line_height; src_y < VBE_mode_info->height; src_y++) {
        uint64_t dst_y = src_y - line_height;
        
        // Calcular offset de la línea fuente y destino
        uint64_t src_offset = src_y * VBE_mode_info->pitch;
        uint64_t dst_offset = dst_y * VBE_mode_info->pitch;
        
        memcpy(framebuffer + dst_offset, framebuffer + src_offset, VBE_mode_info->pitch);
    }
    
    // Limpiar la última línea
    uint64_t last_line_start = VBE_mode_info->height - line_height;
    fillRectangle(0, last_line_start, VBE_mode_info->width, VBE_mode_info->height, BKG_COLOR);
}

void newLine(void) {
    cursor_x = 0;
    uint64_t step_y = (uint64_t)text_size * FONT_HEIGHT;

    if (cursor_y + step_y < VBE_mode_info->height) {
        // Hay espacio para avanzar una línea
        cursor_y += step_y;
        fillRectangle(0, cursor_y, VBE_mode_info->width, cursor_y + step_y, BKG_COLOR);
    } else {
        scrollUp();
        cursor_y = VBE_mode_info->height - step_y;
    }
}

static void updateCursor(void) {
    if (!isValid(cursor_x + (uint64_t)FONT_WIDTH * text_size - 1, cursor_y + (uint64_t)FONT_HEIGHT * text_size - 1)) {
        newLine();
    }
}

static void moveCursorRight(void) {
    uint64_t step_x = (uint64_t)FONT_WIDTH * text_size;
    if (cursor_x + step_x < VBE_mode_info->width) {
        cursor_x += step_x;
    } else {
        newLine();
    }
}

static void moveCursorLeft(void) {
    uint64_t step_x = (uint64_t)FONT_WIDTH * text_size;
    if (cursor_x >= step_x) {
        cursor_x -= step_x;
    } else {
        uint64_t step_y = (uint64_t)FONT_HEIGHT * text_size;
        if (cursor_y >= step_y) {
            cursor_y -= step_y;
            uint64_t cols = VBE_mode_info->width / step_x;
            if (cols == 0) cols = 1;
            cursor_x = (cols - 1) * step_x;
        } else {
            cursor_x = 0;
        }
    }
}

static void deleteChar(void) {
    moveCursorLeft();
    uint64_t h = (uint64_t)FONT_HEIGHT * text_size;
    uint64_t w = (uint64_t)FONT_WIDTH * text_size;
    for (uint64_t yy = 0; yy < h; yy++) {
        for (uint64_t xx = 0; xx < w; xx++) {
            putPixel(BKG_COLOR, cursor_x + xx, cursor_y + yy);
        }
    }
}

void vdPutChar(uint8_t c, uint32_t color) {
    if (!text_mode) return;
    if (c == '\n') { newLine(); return; }
    if (c == '\b') { deleteChar(); return; }
    drawChar((uint32_t)cursor_x, cursor_y, c, color, text_size);
    moveCursorRight();
    updateCursor();
}

void vdPrint(const char *str, uint32_t color) {
    if (!text_mode || str == NULL) return;
    for (size_t i = 0; str[i] != '\0'; i++) {
        vdPutChar((uint8_t)str[i], color);
    }
}

void vdIncreaseTextSize(void) {
    if (text_size < MAX_SIZE && text_mode) text_size++;
    updateCursor();
}

void vdDecreaseTextSize(void) {
    if (text_size > 1 && text_mode) text_size--;
    updateCursor();
}

void vdClear(void) {
    if (!text_mode) return;
    cursor_x = 0;
    cursor_y = 0;
    fillRectangle(0, 0, VBE_mode_info->width, VBE_mode_info->height, BKG_COLOR);
}





/*---------------------------------- MODO VIDEO ----------------------------------*/




void drawLine(uint64_t x0, uint64_t y0, uint64_t x1, uint64_t y1, uint32_t color) {
    // Bresenham con enteros de 64 bits para evitar overflow en pantallas grandes
    int64_t ix0 = (int64_t)x0, iy0 = (int64_t)y0, ix1 = (int64_t)x1, iy1 = (int64_t)y1;
    int64_t dx = kabs((int)(ix1 - ix0));
    int64_t sx = ix0 < ix1 ? 1 : -1;
    int64_t dy = -kabs((int)(iy1 - iy0));
    int64_t sy = iy0 < iy1 ? 1 : -1;
    int64_t err = dx + dy;

    while (1) {
        if (isValid((uint64_t)ix0, (uint64_t)iy0)) putPixel(color, (uint64_t)ix0, (uint64_t)iy0);
        if (ix0 == ix1 && iy0 == iy1) break;
        int64_t e2 = 2 * err;
        if (e2 >= dy) { err += dy; ix0 += sx; }
        if (e2 <= dx) { err += dx; iy0 += sy; }
    }
}

void fillRectangle(uint64_t x0, uint64_t y0, uint64_t x1, uint64_t y1, uint32_t color) {
    if (x1 <= x0 || y1 <= y0) return;
    if (x0 >= VBE_mode_info->width || y0 >= VBE_mode_info->height) return;

    // Clamp a los bordes de pantalla
    if (x1 > VBE_mode_info->width) x1 = VBE_mode_info->width;
    if (y1 > VBE_mode_info->height) y1 = VBE_mode_info->height;

    uint32_t bytes_per_pixel = (VBE_mode_info->bpp + 7) / 8;
    uint8_t *framebuffer = (uint8_t *)(uintptr_t)VBE_mode_info->framebuffer;
    uint64_t pitch = VBE_mode_info->pitch;

    for (uint64_t y = y0; y < y1; y++) {
        uint64_t row_offset = y * pitch + x0 * bytes_per_pixel;
        for (uint64_t x = x0; x < x1; x++) {
            uint64_t off = row_offset + (x - x0) * bytes_per_pixel;
            for (uint32_t b = 0; b < bytes_per_pixel; b++) {
                framebuffer[off + b] = (color >> (8 * b)) & 0xFF;
            }
        }
    }
}

void drawRectangle(uint64_t x0, uint64_t y0, uint64_t x1, uint64_t y1, uint32_t color) {
    if (x1 <= x0 || y1 <= y0) return;
    drawLine(x0, y0, x1 - 1, y0, color); // arriba
    drawLine(x0, y0, x0, y1 - 1, color); // izquierda
    drawLine(x0, y1 - 1, x1 - 1, y1 - 1, color); // abajo
    drawLine(x1 - 1, y0, x1 - 1, y1 - 1, color); // derecha
}

void drawFilledRect(uint32_t x, uint32_t y, uint32_t width, uint32_t height, uint32_t color) {
    fillRectangle(x, y, (uint64_t)x + width, (uint64_t)y + height, color);
}

void drawCircle(uint64_t x_center, uint64_t y_center, uint64_t radius, uint32_t color) {
    int64_t x = (int64_t)radius;
    int64_t y = 0;
    int64_t err = 0;

    while (x >= y) {
        putPixel(color, x_center + x, y_center + y);
        putPixel(color, x_center + y, y_center + x);
        putPixel(color, x_center - y, y_center + x);
        putPixel(color, x_center - x, y_center + y);
        putPixel(color, x_center - x, y_center - y);
        putPixel(color, x_center - y, y_center - x);
        putPixel(color, x_center + y, y_center - x);
        putPixel(color, x_center + x, y_center - y);

        if (err <= 0) {
            y += 1;
            err += 2 * y + 1;
        }
        if (err > 0) {
            x -= 1;
            err -= 2 * x + 1;
        }
    }
}



void drawChar(uint32_t x, uint32_t y, uint8_t c, uint32_t color, uint64_t size) {
    if (c >= 128) return;
    if (size == 0) size = 1;

    for (int i = 0; i < FONT_HEIGHT; i++) {
        uint8_t line = font[c][i];
        for (int j = 0; j < FONT_WIDTH; j++) {
            if ((line << j) & 0x80) {
                for (uint64_t dy = 0; dy < size; dy++) {
                    for (uint64_t dx = 0; dx < size; dx++) {
                        putPixel(color, x + (uint64_t)j * size + dx, y + (uint64_t)i * size + dy);
                    }
                }
            }
        }
    }
}

void drawString(const char *str, uint64_t x, uint64_t y, uint32_t color, uint64_t size) {
    if (str == NULL) return;
    if (size == 0) size = 1;
    for (size_t i = 0; str[i] != '\0'; i++) {
        drawChar((uint32_t)(x + (uint64_t)FONT_WIDTH * size * i), (uint32_t)y, (uint8_t)str[i], color, size);
    }
}

void fillCircle(uint64_t x_center, uint64_t y_center, uint64_t radius, uint32_t color){
    uint64_t x0 = (x_center >= radius) ? x_center - radius : 0;
    uint64_t y0 = (y_center >= radius) ? y_center - radius : 0;
    uint64_t x1 = x_center + radius;
    uint64_t y1 = y_center + radius;

    for (int x = x0; x <= x1; x++) {
        for (int y = y0; y <= y1; y++) {
            int dx = x - x_center;
            int dy = y - y_center;
            if (dx*dx + dy*dy <= radius*radius) { // si esta adentro del circulo
                putPixel(color, x, y);
            }
        }
    }
}

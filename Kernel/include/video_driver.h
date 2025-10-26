#ifndef VIDEO_DRIVER_H
#define VIDEO_DRIVER_H

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

/* =============================
 * FUNCIONES DE PIXELES Y VIDEO
 * ============================= */

// Dibuja un píxel en pantalla en la posición (x, y)
void putPixel(uint32_t hexColor, uint64_t x, uint64_t y);

// Obtiene dimensiones actuales de la pantalla
uint16_t getScreenHeight(void);
uint16_t getScreenWidth(void);

/* =============================
 * FUNCIONES DE MODO TEXTO
 * ============================= */

void enableTextMode(void);
void disableTextMode(void);

void vdSetTextSize(uint8_t size);
uint8_t vdGetTextSize(void);
void vdIncreaseTextSize(void);
void vdDecreaseTextSize(void);

void vdPutChar(uint8_t ch, uint32_t color);
void vdPrint(const char *str, uint32_t color);
void vdClear(void);
void newLine(void);

/* =============================
 * FUNCIONES DE MODO GRÁFICO / VIDEO
 * ============================= */

// Dibuja un carácter (glyph) en pantalla desde la fuente
void drawGlyph(uint32_t x, uint32_t y, uint8_t c, uint32_t color, uint64_t size);

// Dibuja una cadena de texto en pantalla
void drawString(const char *str, uint64_t x, uint64_t y, uint32_t color, uint64_t size);

// Dibuja una línea desde (x0, y0) hasta (x1, y1)
void drawLine(uint64_t x0, uint64_t y0, uint64_t x1, uint64_t y1, uint32_t color);

// Dibuja un rectángulo vacío con esquinas (x0, y0) y (x1, y1)
void drawRectangle(uint64_t x0, uint64_t y0, uint64_t x1, uint64_t y1, uint32_t color);

// Dibuja un rectángulo relleno
void fillRectangle(uint64_t x0, uint64_t y0, uint64_t x1, uint64_t y1, uint32_t color);

// Dibuja un círculo con centro y radio especificado
void drawCircle(uint64_t x_center, uint64_t y_center, uint64_t radius, uint32_t color);

// Funciones de texto en modo video
void vdPrint(const char * str, uint32_t color);
void vdPutChar(uint8_t ch, uint32_t color);
void vdClear(void);
void newLine(void);
void vdIncreaseTextSize(void);
void vdDecreaseTextSize(void);

#endif // VIDEO_DRIVER_H

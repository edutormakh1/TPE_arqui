#ifndef VGA_DRIVER_H
#define VGA_DRIVER_H

#include <stdint.h>

// VGA Mode Information Structure
typedef struct {
    uint16_t width;
    uint16_t height;
    uint8_t bpp;        // bits per pixel
    uint8_t is_graphics; // 1 if graphics mode, 0 if text mode
} vga_mode_info_t;

// Basic VGA functions
int vga_set_mode_13h(void);
void vga_disable_display(void);
void vga_enable_display(void);

// Palette functions
void vga_set_palette_entry(uint8_t index, uint8_t red, uint8_t green, uint8_t blue);
void vga_set_default_palette(void);

// Drawing functions
void vga_put_pixel(uint16_t x, uint16_t y, uint8_t color);
uint8_t vga_get_pixel(uint16_t x, uint16_t y);
void vga_clear_screen(uint8_t color);
void vga_draw_rectangle(uint16_t x, uint16_t y, uint16_t width, uint16_t height, uint8_t color);
void vga_draw_line(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1, uint8_t color);

// Mode information
vga_mode_info_t vga_get_mode_info(void);
int vga_is_graphics_mode(void);

// Standard VGA Colors (for 256-color mode)
#define VGA_COLOR_BLACK         0
#define VGA_COLOR_BLUE          1
#define VGA_COLOR_GREEN         2
#define VGA_COLOR_CYAN          3
#define VGA_COLOR_RED           4
#define VGA_COLOR_MAGENTA       5
#define VGA_COLOR_BROWN         6
#define VGA_COLOR_LIGHT_GRAY    7
#define VGA_COLOR_DARK_GRAY     8
#define VGA_COLOR_LIGHT_BLUE    9
#define VGA_COLOR_LIGHT_GREEN   10
#define VGA_COLOR_LIGHT_CYAN    11
#define VGA_COLOR_LIGHT_RED     12
#define VGA_COLOR_LIGHT_MAGENTA 13
#define VGA_COLOR_YELLOW        14
#define VGA_COLOR_WHITE         15

#endif // VGA_DRIVER_H
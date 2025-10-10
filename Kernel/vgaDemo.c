#include <vgaDriver.h>
#include <lib.h>

// Demo function to test VGA graphics mode
void vga_demo() {
    // Initialize VGA Mode 13h (320x200x256 colors)
    if (vga_set_mode_13h() != 0) {
        return; // Failed to set mode
    }
    
    // Set up the default palette
    vga_set_default_palette();
    
    // Clear screen to black
    vga_clear_screen(VGA_COLOR_BLACK);
    
    // Draw some colored rectangles
    vga_draw_rectangle(10, 10, 50, 30, VGA_COLOR_RED);
    vga_draw_rectangle(70, 10, 50, 30, VGA_COLOR_GREEN);
    vga_draw_rectangle(130, 10, 50, 30, VGA_COLOR_BLUE);
    vga_draw_rectangle(190, 10, 50, 30, VGA_COLOR_YELLOW);
    
    // Draw some lines
    vga_draw_line(0, 0, 319, 199, VGA_COLOR_WHITE);
    vga_draw_line(0, 199, 319, 0, VGA_COLOR_WHITE);
    vga_draw_line(160, 0, 160, 199, VGA_COLOR_CYAN);
    vga_draw_line(0, 100, 319, 100, VGA_COLOR_MAGENTA);
    
    // Draw a pattern with individual pixels
    for (int x = 0; x < 320; x += 4) {
        for (int y = 50; y < 150; y += 4) {
            uint8_t color = (x + y) % 16; // Use first 16 colors
            vga_put_pixel(x, y, color);
        }
    }
    
    // Draw a border around the screen
    for (int x = 0; x < 320; x++) {
        vga_put_pixel(x, 0, VGA_COLOR_LIGHT_GRAY);
        vga_put_pixel(x, 199, VGA_COLOR_LIGHT_GRAY);
    }
    for (int y = 0; y < 200; y++) {
        vga_put_pixel(0, y, VGA_COLOR_LIGHT_GRAY);
        vga_put_pixel(319, y, VGA_COLOR_LIGHT_GRAY);
    }
}
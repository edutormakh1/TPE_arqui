#include <vgaDriver.h>
#include <lib.h>

// VGA Hardware Ports
#define VGA_MISC_WRITE      0x3C2
#define VGA_MISC_READ       0x3CC
#define VGA_SEQ_INDEX       0x3C4
#define VGA_SEQ_DATA        0x3C5
#define VGA_GC_INDEX        0x3CE
#define VGA_GC_DATA         0x3CF
#define VGA_CRTC_INDEX      0x3D4
#define VGA_CRTC_DATA       0x3D5
#define VGA_ATTR_INDEX      0x3C0
#define VGA_ATTR_DATA_WRITE 0x3C0
#define VGA_ATTR_DATA_READ  0x3C1
#define VGA_INPUT_STATUS    0x3DA
#define VGA_DAC_MASK        0x3C6
#define VGA_DAC_WRITE_INDEX 0x3C8
#define VGA_DAC_READ_INDEX  0x3C7
#define VGA_DAC_DATA        0x3C9

// VGA Memory
#define VGA_MEMORY          0xA0000

// Current VGA mode info
static vga_mode_info_t current_mode = {0, 0, 0, 0};

// Function to write to indexed VGA registers
static void vga_write_register(uint16_t port, uint8_t index, uint8_t value) {
    outb(port, index);
    outb(port + 1, value);
}

// Function to read from indexed VGA registers
static uint8_t vga_read_register(uint16_t port, uint8_t index) {
    outb(port, index);
    return inb(port + 1);
}

// Function to write to attribute controller (special case)
static void vga_write_attribute(uint8_t index, uint8_t value) {
    inb(VGA_INPUT_STATUS);  // Reset flip-flop
    outb(VGA_ATTR_INDEX, index);
    outb(VGA_ATTR_DATA_WRITE, value);
}

// Disable VGA display (blank screen)
void vga_disable_display() {
    // Turn off screen by setting bit 5 of Sequencer register 1
    uint8_t seq1 = vga_read_register(VGA_SEQ_INDEX, 0x01);
    vga_write_register(VGA_SEQ_INDEX, 0x01, seq1 | 0x20);
}

// Enable VGA display
void vga_enable_display() {
    // Turn on screen by clearing bit 5 of Sequencer register 1
    uint8_t seq1 = vga_read_register(VGA_SEQ_INDEX, 0x01);
    vga_write_register(VGA_SEQ_INDEX, 0x01, seq1 & ~0x20);
}

// Unlock CRTC registers
static void vga_unlock_crtc() {
    // Clear bit 7 of CRTC register 0x11 to unlock registers 0-7
    uint8_t crtc11 = vga_read_register(VGA_CRTC_INDEX, 0x11);
    vga_write_register(VGA_CRTC_INDEX, 0x11, crtc11 & 0x7F);
}

// Lock CRTC registers
static void vga_lock_crtc() {
    // Set bit 7 of CRTC register 0x11 to lock registers 0-7
    uint8_t crtc11 = vga_read_register(VGA_CRTC_INDEX, 0x11);
    vga_write_register(VGA_CRTC_INDEX, 0x11, crtc11 | 0x80);
}

// Mode 13h register values (320x200x256 colors)
static uint8_t mode13h_registers[] = {
    // Miscellaneous Output Register
    0x63,
    
    // Sequencer Registers
    0x03, 0x01, 0x0F, 0x00, 0x0E,
    
    // CRTC Registers  
    0x5F, 0x4F, 0x50, 0x82, 0x54, 0x80, 0xBF, 0x1F,
    0x00, 0x41, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x9C, 0x8E, 0x8F, 0x28, 0x40, 0x96, 0xB9, 0xA3,
    0xFF,
    
    // Graphics Controller Registers
    0x00, 0x00, 0x00, 0x00, 0x00, 0x40, 0x05, 0x0F,
    0xFF,
    
    // Attribute Controller Registers
    0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
    0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F,
    0x41, 0x00, 0x0F, 0x00, 0x00
};

// Initialize VGA Mode 13h (320x200x256 colors)
int vga_set_mode_13h() {
    int i = 0;
    
    // Disable display
    vga_disable_display();
    
    // Unlock CRTC registers
    vga_unlock_crtc();
    
    // Write Miscellaneous Output Register
    outb(VGA_MISC_WRITE, mode13h_registers[i++]);
    
    // Write Sequencer Registers (5 registers: 0-4)
    for (int reg = 0; reg < 5; reg++) {
        vga_write_register(VGA_SEQ_INDEX, reg, mode13h_registers[i++]);
    }
    
    // Write CRTC Registers (25 registers: 0-24)
    for (int reg = 0; reg < 25; reg++) {
        vga_write_register(VGA_CRTC_INDEX, reg, mode13h_registers[i++]);
    }
    
    // Write Graphics Controller Registers (9 registers: 0-8)
    for (int reg = 0; reg < 9; reg++) {
        vga_write_register(VGA_GC_INDEX, reg, mode13h_registers[i++]);
    }
    
    // Write Attribute Controller Registers (21 registers: 0-20)
    inb(VGA_INPUT_STATUS);  // Reset flip-flop
    for (int reg = 0; reg < 21; reg++) {
        vga_write_attribute(reg, mode13h_registers[i++]);
    }
    
    // Enable palette
    inb(VGA_INPUT_STATUS);
    outb(VGA_ATTR_INDEX, 0x20);
    
    // Set DAC mask
    outb(VGA_DAC_MASK, 0xFF);
    
    // Lock CRTC registers
    vga_lock_crtc();
    
    // Enable display
    vga_enable_display();
    
    // Set current mode info
    current_mode.width = 320;
    current_mode.height = 200;
    current_mode.bpp = 8;
    current_mode.is_graphics = 1;
    
    // Clear screen
    vga_clear_screen(0);
    
    return 0;
}

// Set a single palette entry
void vga_set_palette_entry(uint8_t index, uint8_t red, uint8_t green, uint8_t blue) {
    outb(VGA_DAC_WRITE_INDEX, index);
    outb(VGA_DAC_DATA, red >> 2);    // VGA uses 6-bit color values
    outb(VGA_DAC_DATA, green >> 2);
    outb(VGA_DAC_DATA, blue >> 2);
}

// Set default VGA palette (256 colors)
void vga_set_default_palette() {
    // Set standard 16 colors
    vga_set_palette_entry(0, 0, 0, 0);         // Black
    vga_set_palette_entry(1, 0, 0, 170);       // Blue
    vga_set_palette_entry(2, 0, 170, 0);       // Green
    vga_set_palette_entry(3, 0, 170, 170);     // Cyan
    vga_set_palette_entry(4, 170, 0, 0);       // Red
    vga_set_palette_entry(5, 170, 0, 170);     // Magenta
    vga_set_palette_entry(6, 170, 85, 0);      // Brown
    vga_set_palette_entry(7, 170, 170, 170);   // Light Gray
    vga_set_palette_entry(8, 85, 85, 85);      // Dark Gray
    vga_set_palette_entry(9, 85, 85, 255);     // Light Blue
    vga_set_palette_entry(10, 85, 255, 85);    // Light Green
    vga_set_palette_entry(11, 85, 255, 255);   // Light Cyan
    vga_set_palette_entry(12, 255, 85, 85);    // Light Red
    vga_set_palette_entry(13, 255, 85, 255);   // Light Magenta
    vga_set_palette_entry(14, 255, 255, 85);   // Yellow
    vga_set_palette_entry(15, 255, 255, 255);  // White
    
    // Fill the rest with a grayscale gradient
    for (int i = 16; i < 256; i++) {
        uint8_t gray = i;
        vga_set_palette_entry(i, gray, gray, gray);
    }
}

// Put a pixel in Mode 13h
void vga_put_pixel(uint16_t x, uint16_t y, uint8_t color) {
    if (x >= current_mode.width || y >= current_mode.height) {
        return;  // Out of bounds
    }
    
    uint8_t *framebuffer = (uint8_t *)VGA_MEMORY;
    framebuffer[y * current_mode.width + x] = color;
}

// Get a pixel in Mode 13h
uint8_t vga_get_pixel(uint16_t x, uint16_t y) {
    if (x >= current_mode.width || y >= current_mode.height) {
        return 0;  // Out of bounds
    }
    
    uint8_t *framebuffer = (uint8_t *)VGA_MEMORY;
    return framebuffer[y * current_mode.width + x];
}

// Clear screen with specified color
void vga_clear_screen(uint8_t color) {
    uint8_t *framebuffer = (uint8_t *)VGA_MEMORY;
    for (int i = 0; i < current_mode.width * current_mode.height; i++) {
        framebuffer[i] = color;
    }
}

// Draw a rectangle
void vga_draw_rectangle(uint16_t x, uint16_t y, uint16_t width, uint16_t height, uint8_t color) {
    for (uint16_t dy = 0; dy < height; dy++) {
        for (uint16_t dx = 0; dx < width; dx++) {
            vga_put_pixel(x + dx, y + dy, color);
        }
    }
}

// Draw a line (simple Bresenham algorithm)
void vga_draw_line(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1, uint8_t color) {
    int dx = x1 > x0 ? x1 - x0 : x0 - x1;
    int dy = y1 > y0 ? y1 - y0 : y0 - y1;
    int sx = x0 < x1 ? 1 : -1;
    int sy = y0 < y1 ? 1 : -1;
    int err = dx - dy;
    
    while (1) {
        vga_put_pixel(x0, y0, color);
        
        if (x0 == x1 && y0 == y1) break;
        
        int e2 = 2 * err;
        if (e2 > -dy) {
            err -= dy;
            x0 += sx;
        }
        if (e2 < dx) {
            err += dx;
            y0 += sy;
        }
    }
}

// Get current mode information
vga_mode_info_t vga_get_mode_info() {
    return current_mode;
}

// Check if current mode is graphics mode
int vga_is_graphics_mode() {
    return current_mode.is_graphics;
}
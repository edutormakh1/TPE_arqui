#include <stdio.h>
#include "include/video_driver.h"
#include "include/time.h"
#include "include/sound.h"
#include "include/keyboard.h"
#include "include/SyscallDispatcher.h"

#define MIN_CHAR 0
#define MAX_CHAR 256


// El orden de la tabla DEBE coincidir con los números en Userland/Asm/userlib.asm
// 0: regs, 1: time, 2: date, 3: read, 4: write, 5: inc_font, 6: dec_font, 7: beep,
// 8: screen_size, 9: circle, 10: rectangle, 11: line, 12: draw_string, 13: clear,
// 14: speaker_start, 15: speaker_stop, 16: textmode, 17: videomode, 18: putpixel,
// 19: key_status, 20: sleep, 21: clear_input_buffer, 22: ticks
void * syscalls[SYSCALLS_COUNT] = {
    &sys_regs,               // 0
    &sys_time,               // 1
    &sys_date,               // 2
    &sys_read,               // 3
    &sys_write,              // 4
    &sys_increase_fontsize,  // 5
    &sys_decrease_fontsize,  // 6
    &sys_beep,               // 7
    &sys_screensize,         // 8
    &sys_circle,             // 9
    &sys_rectangle,          // 10
    &sys_draw_line,          // 11
    &sys_draw_string,        // 12
    &sys_clear,              // 13
    &sys_speaker_start,      // 14
    &sys_speaker_stop,       // 15
    &sys_textmode,           // 16
    &sys_videomode,          // 17
    &sys_putpixel,           // 18
    &sys_key_status,         // 19
    &sys_sleep,              // 20
    &sys_clear_input_buffer, // 21
    &sys_ticks               // 22
};

static uint64_t sys_write(uint64_t fd, const char * buf, uint64_t count) {
    if (fd != STDOUT && fd != STDERR) {
        return 0;
    }
    uint32_t color = fd == STDERR ? 0xFF0000 : 0xFFFFFF; // Rojo para STDERR, blanco para STDOUT
   

    for(int i=0; i < count; i++) {
        vdPutChar(buf[i], color);
    }
    return count;
}
static uint64_t sys_read(char * buf, uint64_t count) {
   return read_keyboard_buffer(buf, count);
}

static void sys_date(uint8_t * buffer) {
    get_date(buffer);
}

static void sys_time(uint8_t * buffer) {
    get_time(buffer);
}
static uint64_t sys_regs(char * buffer) {
    return copyRegisters(buffer);
}

static void sys_clear() {
    vdClear();
}
//checkear bien los nombres (text y fontsize)
static void sys_increase_fontsize() {
    vdIncreaseTextSize();
}
static void sys_decrease_fontsize() {
    vdDecreaseTextSize();
}
//sonido para el juego

static void sys_beep(uint32_t freq_hz, uint64_t duration) {
    beep(freq_hz, duration);
    
}
    
//obtengo el tamaño de la pantalla
static void sys_screensize(uint32_t * width, uint32_t * height) {
    *width= getScreenWidth();
    *height= getScreenHeight();
}
// pos: [x_center, y_center, radius]
static void sys_circle(uint64_t fill, uint64_t * pos, uint32_t color) {
    if(fill){
        fillCircle(pos[0],pos[1], pos[2], color); 
    }else{
        drawCircle(pos[0],pos[1], pos[2], color);
    }
    
}
// pos: [x0, y0, x1, y1]
static void sys_rectangle(uint64_t fill, uint64_t * pos, uint32_t color) {
    if(fill){
        fillRectangle(pos[0], pos[1], pos[2], pos[3], color);
    }else{
        drawRectangle(pos[0], pos[1], pos[2], pos[3], color);
    }
}
//pos: [x0, y0, x1, y1]
static void sys_draw_line(uint64_t * pos, uint32_t color) {  
    drawLine(pos[0], pos[1], pos[2], pos[3], color);
}
//pos: [x, y, size]
static void sys_draw_string(const char * buf, uint64_t * pos, uint32_t color) {
    drawString(buf, pos[0], pos[1], color, pos[2]);
}



static void sys_speaker_start(uint32_t freq_hz) {
    speaker_start_tone(freq_hz);
   
}
static void sys_speaker_stop() {
     speaker_off();
}

static void sys_textmode() {
    enableTextMode();
}
static void sys_videomode() {
    disableTextMode();
}
static void sys_putpixel(uint32_t hexColor, uint64_t x, uint64_t y) {
    putPixel(hexColor, x, y);
}
static uint64_t sys_key_status(char c) {
    return isPressedKey(c);
}
static void sys_sleep(uint64_t miliseconds) {
    sleep(miliseconds);
}
static void sys_clear_input_buffer() {
    clear_buffer();
}
static uint64_t sys_ticks() {
    return ticks_elapsed();
}



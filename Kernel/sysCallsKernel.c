#include <stdint.h>
#include "videoDriver.h"
#include "keyboard.h"
#include "sound.h"


#define MIN_CHAR 0
#define MAX_CHAR 256

void * syscalls[] = {
    &sys_regs,
    &sys_time,
    &sys_date,
    &sys_read,
    &sys_write,
    &sys_increase_fontsize,
    &sys_decrease_fontsize,
    &sys_beep,
    &sys_screensize,
    &sys_circle,
    &sys_rectangle,
    &sys_draw_line,
    &sys_draw_string,
    &sys_clear,
    &sys_speaker_start,
    &sys_speaker_stop,
    &sys_textmode,
    &sys_videomode,
    &sys_putpixel,
    &sys_key_status,
    &sys_sleep,
    &sys_clear_input_buffer,
    &sys_ticks
};

// info: [x0, y0, size]
static void sys_draw_string(const char * buf, uint64_t * info, uint32_t color) {
    drawString(buf, info[0], info[1], color, info[2]);
}

// info: [x0, y0, x1, y1]
static void sys_draw_line(uint64_t * info, uint32_t color) {
    drawLine(info[0], info[1], info[2], info[3], color);
}

// info: [x0, y0, x1, y1]
static void sys_rectangle(uint64_t fill, uint64_t * info, uint32_t color) {
    if (fill) {
        fillRectangle(info[0], info[1], info[2], info[3], color);
    } else {
        drawRectangle(info[0], info[1], info[2], info[3], color);
    }
}
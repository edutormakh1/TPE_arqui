#ifndef GRAPHICS_H
#define GRAPHICS_H

#include "tron.h"

#define PLAYER1_COLOR 0x00FFFF // Verde cian
#define PLAYER2_COLOR 0xFF00FF // Magenta
#define GRID_COLOR 0x333333    // Gris oscuro
#define BACKGROUND_COLOR 0x000000 // Negro
#define TEXT_COLOR 0xFFFFFF       // Blanco

void draw_board();
void draw_moto(int x, int y, int player_id);
void clear_screen();
void init_graphics();
uint32_t get_width();
uint32_t get_height();
int starting_screen();

#endif
#ifndef GRAPHICS_H
#define GRAPHICS_H

#include "config.h"
#include "tron.h"
#include <stdint.h>


#define PLAYER1_COLOR 0x00FFFF    // Verde cian
#define PLAYER2_COLOR 0xFF00FF    // Magenta
#define DANGER_ZONE_COLOR 0x980000 // Naranja/Rojo (franja peligrosa nivel 2)
#define EXTRA_SPEED_ZONE_COLOR 0x00FF00 // Verde (franja de velocidad extra nivel 3)
#define GRID_COLOR 0xFFFFFF       // Gris verdoso (borde del tablero)
#define GRID_LINE_COLOR 0x696E69  // Gris verdoso (líneas del grid)
#define BACKGROUND_COLOR 0x000000 // Negro (fondo de la pantalla)
#define BOARD_BACKGROUND_COLOR 0x214766 // Azul oscuro (fondo del tablero)
#define TEXT_COLOR 0xFFFFFF       // Blanco

void init_graphics();
uint32_t get_width();
uint32_t get_height();


int starting_screen();
void show_game_over_screen(int mode_select, int winner, int score);


void clear_screen();
void draw_board(int board[BOARD_HEIGHT][BOARD_WIDTH]);
void draw_moto_cell(int x, int y, int player_id, uint32_t cell_size, uint32_t offset_x, uint32_t offset_y);
void draw_board_border(uint32_t offset_x, uint32_t offset_y, uint32_t cell_size);
void draw_grid(uint32_t offset_x, uint32_t offset_y, uint32_t cell_size);


void draw_game_info(int mode_select, int score);
void draw_controls_help(int mode_select);
void draw_level_info();
void update_square(Moto *moto, uint32_t color); // Dibuja de forma incremental la celda actual de la moto


void animate_collision(int x, int y, uint32_t cell_size);

// Dibuja una celda del tablero en (x,y) con color absoluto, usando el mismo centrado/escala del tablero
void draw_cell_xy(int x, int y, uint32_t color);

#endif
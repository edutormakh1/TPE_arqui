#ifndef TRON_H
#define TRON_H

#include "config.h"

// Estructura para las motos
typedef struct {
    int x;          //  X
    int y;          //  Y
    char direction; // (Up, Down, Left, Right)
    int active;     // 1 si está activa, 0 si está eliminada
} Moto;


void initialize_game(Moto *player1, Moto *player2);
void update_game(Moto *player1, Moto *player2);
int check_collision(Moto *moto);
void draw_board();

#endif
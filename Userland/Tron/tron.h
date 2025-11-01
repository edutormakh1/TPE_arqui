#ifndef TRON_H
#define TRON_H

#include "config.h"
#include <stdint.h>

// Velocidad inicial por defecto (unidades/celdas por frame)
#define INITIAL_SPEED 1.0


// Estructura para las motos
typedef struct {
    int x;          //  X
    int y;          //  Y
    char direction; // (Up, Down, Left, Right)
    int active;     // 1 si está activa, 0 si está eliminada
    void (*draw)(struct Moto *);
    double speed;
} Moto;

void tron_main();
void initialize_game();
void game_loop();
void process_input();
void update_game();
void move_moto(Moto *moto, int player_id);
void change_direction(Moto *moto, char new_direction);
int check_collision_at(int x, int y);
int get_score();
int get_board_value(int x, int y);




#endif
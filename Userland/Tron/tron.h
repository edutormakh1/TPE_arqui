#ifndef TRON_H
#define TRON_H

#include "config.h"
#include <stdint.h>

// Velocidad inicial por defecto (unidades/celdas por frame)
#define INITIAL_SPEED 1

// Estructura para las motos
typedef struct {
    int x;          //  X
    int y;          //  Y
    char direction; // (Up, Down, Left, Right)
    int active;     // 1 si está activa, 0 si está eliminada
    void (*draw)(struct Moto *);
    int speed;
} Moto;

// =================== FUNCIONES PRINCIPALES (tron.c) ===================
void tron_main();
void game_loop();
void process_input();

#endif
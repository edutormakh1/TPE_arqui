#ifndef LOGICA_H
#define LOGICA_H

#include <stdint.h>
#include "tron.h"
#include "levels.h"

// =================== INICIALIZACIÓN ===================
void initialize_game(int mode);
void reset_round();

// =================== ENTRADA ===================
void process_player_input(int player_id, char key);

// =================== ACTUALIZACIÓN ===================
// Retorna: 0 = juego terminado, 1 = continua normalmente, 2 = ronda reiniciada (necesita redibujar todo)
int update_game();

// =================== ESTADO DEL JUEGO ===================
int get_game_running();
void set_game_running(int running);
int get_winner();
int get_mode_select();

// =================== PUNTAJES ===================
int get_score(); // Puntaje del nivel actual (singleplayer) o score general
int get_total_score(); // Puntaje total acumulado en todos los niveles
int get_current_level(); // Nivel actual (1, 2, o 3)
int get_player1_score();
int get_player2_score();

// =================== ACCESO A DATOS ===================
int* get_board(); // Retorna puntero al tablero para dibujarlo
Moto* get_player1();
Moto* get_player2();
int get_board_value(int x, int y);


#endif
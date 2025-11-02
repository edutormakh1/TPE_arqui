#include "levels.h"
#include "tron.h"
#include "config.h"
#include "../Userlib/userlib.h"

// =================== VARIABLES DE NIVELES ===================
static int current_level = 1;
static int level_score = 0;
static int total_score = 0;

// =================== ZONAS ESPECIALES ===================
static Zone danger_zone = {0, 0, 0, 0, DANGER_ZONE_ID};
static Zone square_zone = {0, 0, 0, 0, DANGER_ZONE_ID};
static Zone square_zone2 = {0, 0, 0, 0, DANGER_ZONE_ID};
static Zone extra_speed_zone = {0, 0, 0, 0, EXTRA_SPEED_ZONE_ID};

// =================== FUNCIONES AUXILIARES ===================
// Marca una zona rectangular en el board con un valor específico
static void mark_zone_in_board(int* board, Zone* zone) {
    int(*board_array)[BOARD_WIDTH] = (int(*)[BOARD_WIDTH])board;
    for (int y = zone->y_start; y <= zone->y_end && y < BOARD_HEIGHT; y++) {
        for (int x = zone->x_start; x <= zone->x_end && x < BOARD_WIDTH; x++) {
            board_array[y][x] = zone->zone_id;
        }
    }
}

// =================== CONFIGURACIÓN DE NIVELES ===================
void level1_config(int* board, Moto* player1) {
    (void)board; // No se usa en nivel 1
    player1->speed = INITIAL_SPEED;
}

void level2_config(int* board, Moto* player1) {
    (void)player1; // No se usa aquí
    
    // Crear franja peligrosa arriba a la derecha
    int danger_zone_width = BOARD_WIDTH / 4;
    danger_zone.x_start = BOARD_WIDTH - danger_zone_width;
    danger_zone.x_end = BOARD_WIDTH - 1;
    danger_zone.y_start = BOARD_HEIGHT / 10;
    int danger_zone_height = BOARD_HEIGHT / 20;
    danger_zone.y_end = danger_zone.y_start + danger_zone_height - 1;
    mark_zone_in_board(board, &danger_zone);
    
    // Crear cuadrado abajo a la derecha
    int square_size = 15;
    int margin_from_bottom = 5;
    int margin_from_right = 5;
    
    square_zone.x_end = BOARD_WIDTH - 1 - margin_from_right;
    square_zone.x_start = square_zone.x_end - square_size + 1;
    square_zone.y_end = BOARD_HEIGHT - 1 - margin_from_bottom;
    square_zone.y_start = square_zone.y_end - square_size + 1;
    mark_zone_in_board(board, &square_zone);
}

void level3_config(int* board, Moto* player1) {
    (void)player1; // No se usa aquí
    
    // Crear cuadrado abajo a la izquierda
    int square_size2 = 30;
    square_zone2.x_start = 0;
    square_zone2.x_end = square_zone2.x_start + square_size2 - 1;
    square_zone2.y_end = BOARD_HEIGHT - 1;
    square_zone2.y_start = square_zone2.y_end - square_size2 + 1;
    mark_zone_in_board(board, &square_zone2);
    
    // Crear zona de velocidad extra abajo a la derecha
    int square_size3 = 15;
    int margin_from_bottom3 = 5;
    int margin_from_right3 = 5;
    
    extra_speed_zone.x_end = BOARD_WIDTH - 1 - margin_from_right3;
    extra_speed_zone.x_start = extra_speed_zone.x_end - square_size3 + 1;
    extra_speed_zone.y_end = BOARD_HEIGHT - 1 - margin_from_bottom3;
    extra_speed_zone.y_start = extra_speed_zone.y_end - square_size3 + 1;
    mark_zone_in_board(board, &extra_speed_zone);
}

void configure_level(int level, int* board, Moto* player1) {
    if (level == 1) {
        level1_config(board, player1);
    } else if (level == 2) {
        level2_config(board, player1);
    } else if (level == 3) {
        level3_config(board, player1);
    }
}

// =================== CONTROL DE NIVELES ===================
int get_current_level(void) {
    return current_level;
}

int get_level_score(void) {
    return level_score;
}

int get_total_score(void) {
    // Si level_score > 0, sumarlo al total (puede pasar si el juego termina sin avanzar)
    // Esto asegura que el puntaje final incluya el nivel actual
    return total_score;
}

void set_level_score(int score) {
    level_score = score;
}

void check_level_speed_changes(int score, Moto* player1) {
    // Nivel 1: aumentar velocidad según score
    if (score >= 100) {
        player1->speed = 2;
    }
    if (score >= 200) {
        player1->speed = 3;
    }
}

int advance_to_next_level(int* board, Moto* player1) {
    (void)board; // Puede usarse en el futuro
    (void)player1; // Puede usarse en el futuro
    
    if (current_level >= MAX_LEVELS) return 0;
    
    // Acumular puntaje del nivel actual antes de avanzar
    total_score += level_score;
    
    // Avanzar nivel
    current_level++;
    level_score = 0;
    
    return 1;
}

void reset_levels_state(void) {
    current_level = 1;
    level_score = 0;
    total_score = 0;
    
    // Resetear zonas
    danger_zone = (Zone){0, 0, 0, 0, DANGER_ZONE_ID};
    square_zone = (Zone){0, 0, 0, 0, DANGER_ZONE_ID};
    square_zone2 = (Zone){0, 0, 0, 0, DANGER_ZONE_ID};
    extra_speed_zone = (Zone){0, 0, 0, 0, EXTRA_SPEED_ZONE_ID};
}

// =================== ACCESO A ZONAS ===================
Zone* get_danger_zone(void) {
    return &danger_zone;
}

Zone* get_extra_speed_zone(void) {
    return &extra_speed_zone;
}


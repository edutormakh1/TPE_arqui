#ifndef LEVELS_H
#define LEVELS_H

#include "config.h"
#include "tron.h"

// =================== ESTRUCTURA DE ZONAS ===================
typedef struct {
    int x_start, x_end;
    int y_start, y_end;
    int zone_id;  // DANGER_ZONE_ID, EXTRA_SPEED_ZONE_ID, etc.
} Zone;

// =================== CONFIGURACIÓN DE NIVELES ===================
// Aplica la configuración del nivel actual (zonas, velocidad inicial, etc.)
// Requiere acceso externo a board y player1
void configure_level(int level, int* board, Moto* player1);

// Avanza al siguiente nivel cuando el jugador muere
// Retorna 1 si avanzó, 0 si no
int advance_to_next_level(int* board, Moto* player1);

// Obtiene información del sistema de niveles
int get_current_level(void);
int get_level_score(void);
int get_total_score(void);

// Verifica cambios de velocidad según el score del nivel
void check_level_speed_changes(int level_score, Moto* player1);

// Obtiene referencias a zonas para colisiones (nivel 2 y 3)
Zone* get_danger_zone(void);
Zone* get_extra_speed_zone(void);

// Resetea el estado de niveles al iniciar un nuevo juego
void reset_levels_state(void);

// Establece el puntaje del nivel (usado por update_level_score)
void set_level_score(int score);

#endif


#include "logica.h"
#include "config.h"
#include "audio.h"
#include "levels.h"
#include "graphics.h"
#include "../Userlib/userlib.h"

// =================== VARIABLES DE ESTADO DEL JUEGO ===================
static int board[BOARD_HEIGHT][BOARD_WIDTH];
static Moto player1;
static Moto player2;
static int game_running = 0;
static int winner = 0; // 0: ninguno, 1: jugador 1, 2: jugador 2
static int mode_select = 0; // 1 si es singleplayer, 2 si es multiplayer
static int p1_points = 0; // Puntos del jugador 1 (multiplayer)
static int p2_points = 0; // Puntos del jugador 2 (multiplayer)

// Variable para rastrear el estado de la zona de velocidad extra
static int saved_normal_speed = 1;        // Velocidad normal guardada antes de aumentar

// =================== DECLARACIONES FORWARD ===================
void increase_speed(Moto *moto, int player_id);

// =================== FUNCIONES AUXILIARES ===================
void board_clean() {
    for (int i = 0; i < BOARD_HEIGHT; i++) {
        for (int j = 0; j < BOARD_WIDTH; j++) {
            board[i][j] = 0;
        }
    }
}

void reset_singleplayer_board() {
    board_clean(); // Limpiar el tablero
    player1.x = BOARD_WIDTH / 2;
    player1.y = BOARD_HEIGHT / 2;
    player1.direction = 'R';
    player1.active = 1;
    // La velocidad se establece en apply_level_config()
    player2.active = 0; // No se usa
    board[player1.y][player1.x] = PLAYER1_ID;
    
    // Resetear estado de la zona de velocidad extra
    saved_normal_speed = INITIAL_SPEED;
    
    // Después de limpiar y marcar la posición del jugador, aplicar configuración del nivel
    // Esto asegura que la franja peligrosa del nivel 2 se dibuje correctamente
    if (mode_select == 1) {
        configure_level(get_current_level(), (int*)board, &player1);
    }
}

void reset_multiplayer_board() {
    player1.x = BOARD_WIDTH / 4;
    player1.y = BOARD_HEIGHT / 2;
    player1.direction = 'R';
    player1.active = 1;
    // Mantener la velocidad acumulada, pero asegurar que sea al menos INITIAL_SPEED
    if (player1.speed < INITIAL_SPEED) player1.speed = INITIAL_SPEED;

    player2.x = 3 * BOARD_WIDTH / 4;
    player2.y = BOARD_HEIGHT / 2;
    player2.direction = 'L';
    player2.active = 1;
    // Mantener la velocidad acumulada, pero asegurar que sea al menos INITIAL_SPEED
    if (player2.speed < INITIAL_SPEED) player2.speed = INITIAL_SPEED;
    
    board[player1.y][player1.x] = PLAYER1_ID;
    board[player2.y][player2.x] = PLAYER2_ID;
}


// =================== FUNCIÓN AUXILIAR PARA CONTAR CELDAS ===================
// Cuenta las celdas ocupadas por un jugador (usado para puntajes)
int count_player_cells(int player_id) {
    int count = 0;
    for (int i = 0; i < BOARD_HEIGHT; i++) {
        for (int j = 0; j < BOARD_WIDTH; j++) {
            if (board[i][j] == player_id) {
                count++;
            }
        }
    }
    return count;
}

// =================== INICIALIZACIÓN ===================
void initialize_game(int mode) {
    mode_select = mode;
    board_clean();
    
    if (mode_select == 1) {
        reset_levels_state();
        reset_singleplayer_board(); 
    } else if (mode_select == 2) {
        // Resetear velocidades al iniciar un nuevo juego multiplayer
        player1.speed = INITIAL_SPEED;
        player2.speed = INITIAL_SPEED;
        reset_multiplayer_board();
        p1_points = 0;
        p2_points = 0;
    }

    game_running = 1;
    winner = 0;
}

void reset_round() {
    board_clean();
    if (mode_select == 2) {
        reset_multiplayer_board();
    }
}

// =================== COLISIONES ===================
int check_collision_at(int x, int y) {
    // Colisión con bordes
    if (x < 0 || x >= BOARD_WIDTH || y < 0 || y >= BOARD_HEIGHT) {
        return 1;
    }
    
    // Colisión con rastros
    // Ignorar EXTRA_SPEED_ZONE_ID en nivel 3 (zona de velocidad extra)
    if (board[y][x] != 0) {
        if (mode_select == 1 && get_current_level() == 3 && board[y][x] == EXTRA_SPEED_ZONE_ID) {
            return 0; // No es colisión si es la zona de velocidad extra en nivel 3
        }
        return 1;
    }
    
    return 0;
}

int check_collision_against_player(int x, int y, int current_player_id) {
    // Colisión con bordes (no es contra otro jugador)
    if (x < 0 || x >= BOARD_WIDTH || y < 0 || y >= BOARD_HEIGHT) {
        return 0;
    }
    
    // Colisión con rastros de otro jugador
    int cell_value = board[y][x];
    if (cell_value != 0 && cell_value != current_player_id) {
        return cell_value; // Retorna el ID del jugador contra el cual chocó
    }
    
    return 0; // No hay colisión contra otro jugador
}

// =================== MOVIMIENTO ===================
void change_direction(Moto *moto, char new_direction) {
    if (!moto->active) return;
    
    // Evitar dirección opuesta
    if ((moto->direction == 'U' && new_direction == 'D') ||
        (moto->direction == 'D' && new_direction == 'U') ||
        (moto->direction == 'L' && new_direction == 'R') ||
        (moto->direction == 'R' && new_direction == 'L')) {
        return;
    }
    
    moto->direction = new_direction;
}

void move_moto(Moto *moto, int player_id) {
    if (!moto->active || !game_running) return;
    
    int new_x = moto->x;
    int new_y = moto->y;
    
    int steps = (moto->speed);
    
    uint32_t color = (player_id == PLAYER1_ID) ? PLAYER1_COLOR : PLAYER2_COLOR;
    uint32_t cell_width = get_width() / BOARD_WIDTH;
    uint32_t cell_height = get_height() / BOARD_HEIGHT;
    uint32_t cell_size = (cell_width < cell_height) ? cell_width : cell_height;

    for (int i = 0; i < steps && moto->active; i++) {
        new_x = moto->x;
        new_y = moto->y;
        
        switch (moto->direction) {
            case 'U': new_y--; break;
            case 'D': new_y++; break;
            case 'L': new_x--; break;
            case 'R': new_x++; break;
        }
        
        // Verificar colisiones paso a paso
        if (mode_select == 2) {
            // En multiplayer, verificar si chocó contra otro jugador
            int collided_player = check_collision_against_player(new_x, new_y, player_id);
            if (collided_player != 0) {
                moto->active = 0;
                // Detener también al otro jugador inmediatamente para evitar que se mueva más
                if (player_id == PLAYER1_ID) {
                    player2.active = 0;
                    p2_points++; // Player 2 gana un punto porque Player 1 chocó contra él
                } else {
                    player1.active = 0;
                    p1_points++; // Player 1 gana un punto porque Player 2 chocó contra él
                }
                increase_speed(moto, player_id);
                play_collision_sound();
                animate_collision(new_x, new_y, cell_size);
                break;
            } else if (check_collision_at(new_x, new_y)) {
                // Chocó contra borde o su propio rastro
                moto->active = 0;
                // Detener también al otro jugador inmediatamente para evitar que se mueva más
                if (player_id == PLAYER1_ID) {
                    player2.active = 0;
                    p2_points++;
                } else {
                    player1.active = 0;
                    p1_points++;
                }
                increase_speed(moto, player_id); //aumenta la velocidad del jugador que gano el punto
                play_collision_sound();
                animate_collision(new_x, new_y, cell_size);
                break;
            }
        } else if (mode_select == 1) {
            // Singleplayer: lógica original
            // (Las características del nivel se aplican en apply_level_config
            if (check_collision_at(new_x, new_y)) {
                moto->active = 0;
                play_collision_sound();
                animate_collision(new_x, new_y, cell_size);
                // Delay para que la animación sea visible antes de continuar
                sys_sleep(50);
                break;
            }
            
            // Verificar si está en la franja peligrosa del nivel 2
            if (get_current_level() == 2) {
                Zone* danger_zone = get_danger_zone();
                // Verificar si la nueva posición está dentro de la franja peligrosa
                int in_danger_zone = (new_x >= danger_zone->x_start && new_x <= danger_zone->x_end &&
                                     new_y >= danger_zone->y_start && new_y <= danger_zone->y_end);
                
                if (in_danger_zone) {
                    // Si está en la franja peligrosa, verificar si ya tiene rastro pintado allí
                    // (si la celda ya tiene PLAYER1_ID, significa que ya pasó antes por ahí)
                    if (board[new_y][new_x] == PLAYER1_ID) {
                        // El jugador está pisando la franja peligrosa Y ya tiene rastro pintado allí -> MUERTE
                        moto->active = 0;
                        play_collision_sound();
                        animate_collision(new_x, new_y, cell_size);
                        // Delay para que la animación sea visible antes de continuar
                        sys_sleep(50);
                        break;
                    }
                }
            }
        }
        
        // Verificar si está en la zona de velocidad extra ANTES de mover (solo en nivel 3)
        if (mode_select == 1 && get_current_level() == 3) {
            Zone* extra_zone = get_extra_speed_zone();
            // Verificar posición anterior
            int was_in_zone = (moto->x >= extra_zone->x_start && moto->x <= extra_zone->x_end &&
                              moto->y >= extra_zone->y_start && moto->y <= extra_zone->y_end);
            // Verificar nueva posición
            int in_extra_speed_zone = (new_x >= extra_zone->x_start && new_x <= extra_zone->x_end &&
                                      new_y >= extra_zone->y_start && new_y <= extra_zone->y_end);
            
            // Detectar transición: de fuera a dentro (entrada)
            if (in_extra_speed_zone && !was_in_zone) {
                // Guardar velocidad normal antes de aumentar
                saved_normal_speed = moto->speed;
                // Aumentar velocidad en 1 (solo una vez)
                if (moto->speed < 10) {
                    moto->speed += 1;
                }
            }
            // Detectar transición: de dentro a fuera (salida)
            else if (!in_extra_speed_zone && was_in_zone) {
                // Restaurar velocidad normal
                moto->speed = saved_normal_speed;
            }
        }
        
        // Avanzar 1 celda, marcar rastro y dibujar incrementalmente
        moto->x = new_x;
        moto->y = new_y;
        board[moto->y][moto->x] = player_id;
        
        update_square(moto, color);
    }
}

void increase_speed(Moto *moto, int player_id) {
    if(player_id == PLAYER1_ID) {
        player2.speed += 1;
    } else {
        player1.speed += 1;
    }
}

void process_player_input(int player_id, char key) {
    Moto *moto = (player_id == PLAYER1_ID) ? &player1 : &player2;
    
    switch (key) {
        case 'w': case 'W':
            if (player_id == PLAYER1_ID) change_direction(moto, 'U');
            break;
        case 's': case 'S':
            if (player_id == PLAYER1_ID) change_direction(moto, 'D');
            break;
        case 'a': case 'A':
            if (player_id == PLAYER1_ID) change_direction(moto, 'L');
            break;
        case 'd': case 'D':
            if (player_id == PLAYER1_ID) change_direction(moto, 'R');
            break;
        case 'i': case 'I':
            if (player_id == PLAYER2_ID && mode_select == 2) change_direction(moto, 'U');
            break;
        case 'k': case 'K':
            if (player_id == PLAYER2_ID && mode_select == 2) change_direction(moto, 'D');
            break;
        case 'j': case 'J':
            if (player_id == PLAYER2_ID && mode_select == 2) change_direction(moto, 'L');
            break;
        case 'l': case 'L':
            if (player_id == PLAYER2_ID && mode_select == 2) change_direction(moto, 'R');
            break;
    }
}

// =================== ACTUALIZACIÓN DEL JUEGO ===================
// Retorna: 0 = juego terminado, 1 = continua normalmente, 2 = ronda reiniciada (necesita redibujar todo)
int update_game() {
    if (!game_running) return 0;
    
    if (mode_select == 1) {
        // MODO SINGLEPLAYER
        move_moto(&player1, PLAYER1_ID);
        
        // Actualizar puntaje del nivel
        update_level_score();
        
        if (!player1.active) {
            // El jugador chocó - verificar si puede avanzar al siguiente nivel
            if (get_current_level() < MAX_LEVELS) {
                
                player1.speed = INITIAL_SPEED; //la velociadad arranca siempre en 1
                if (advance_to_next_level((int*)board, &player1)) {
                    reset_singleplayer_board();
                    return 2; // Ronda reiniciada, necesita redibujar todo
                }
            } else {
                // Está en el último nivel y chocó - game over
                // Acumular puntaje final del nivel actual al total
                int final_level_score = get_level_score();
                set_level_score(final_level_score); // Actualizar level_score
                // El total_score se actualiza cuando se llama get_total_score()
                // que internamente suma level_score si es necesario, pero mejor hacerlo explícito
                winner = 0; // Perdió
                game_running = 0;
                return 0; // Juego terminado
            }
        }
    } else if (mode_select == 2) {
        // MODO MULTIPLAYER
        // Mover player1 primero
        move_moto(&player1, PLAYER1_ID);
        // Si player1 chocó, NO mover player2 (ya está detenido dentro de move_moto)
        if (player1.active) {
            move_moto(&player2, PLAYER2_ID);
        }
        
        // Verificar si alguien llegó a 3 puntos
        if (p1_points >= WINNING_POINTS) {
            winner = 1; // Jugador 1 gana el juego completo
            game_running = 0;
            return 0; // Juego terminado
        } else if (p2_points >= WINNING_POINTS) {
            winner = 2; // Jugador 2 gana el juego completo
            game_running = 0;
            return 0; // Juego terminado
        } else if (!player1.active || !player2.active) {
            // Alguien chocó, pero aún no hay ganador del juego
            // Reiniciar la ronda después de un pequeño delay
            sys_sleep(50);
            reset_round();
            return 2; // Ronda reiniciada, necesita redibujar todo
        }
    }
    return 1; // Continúa el juego normalmente
}

// =================== PUNTAJES ===================
int get_score() {
    if (mode_select != 1) return 0;
    return get_level_score();
}


// Función para actualizar el puntaje del nivel
void update_level_score() {
    if (mode_select != 1) return;
    
    // Contar celdas ocupadas por el jugador (puntaje actual)
    int current_cells = count_player_cells(PLAYER1_ID);
    set_level_score(current_cells);
    
    // Verificar cambios de velocidad según el score
    check_level_speed_changes(current_cells, &player1);
}

int get_player1_score() {
    if (mode_select == 2) {
        return p1_points;
    }
    // En singleplayer, contar celdas (usar función auxiliar)
    return count_player_cells(PLAYER1_ID);
}

int get_player2_score() {
    if (mode_select == 2) {
        return p2_points;
    }
    return 0;
}

// =================== ACCESO AL ESTADO ===================
int get_game_running() {
    return game_running;
}

void set_game_running(int running) {
    game_running = running;
}

int get_winner() {
    return winner;
}

int get_mode_select() {
    return mode_select;
}

int* get_board() {
    return (int*)board;
}

Moto* get_player1() {
    return &player1;
}

Moto* get_player2() {
    return &player2;
}

int get_board_value(int x, int y) {
    if (x < 0 || x >= BOARD_WIDTH || y < 0 || y >= BOARD_HEIGHT) {
        return -1;
    }
    return board[y][x];
}


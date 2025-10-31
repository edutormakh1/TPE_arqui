#include "tron.h"
#include "../Userlib/userlib.h"
#include "graphics.h"
#include "config.h"

// tablero del juego
static int board[BOARD_HEIGHT][BOARD_WIDTH];

//variables del juego
static int game_running=0;
static int winner=0; // 0: ninguno, 1: jugador 1, 2: jugador 2
static int mode_select= 0; //1 si es singleplayer, 2 si es multiplayer


static Moto player1;
static Moto player2;


void tron_main() {
    init_graphics();
    
    int num = starting_screen();
    if (num == 0) {
        return; // Salir del juego
    }   
    mode_select = num;

    initialize_game();
    game_loop();
}

// =================== INICIALIZACIÓN DEL JUEGO ===================
void initialize_game() {
    // Limpiar el tablero
    for (int i = 0; i < BOARD_HEIGHT; i++) {
        for (int j = 0; j < BOARD_WIDTH; j++) {
            board[i][j] = 0;
        }
    }

    if (mode_select == 1) {
        // MODO SINGLEPLAYER: Una moto en el centro
        player1.x = BOARD_WIDTH / 2;
        player1.y = BOARD_HEIGHT / 2;
        player1.direction = 'R';
        player1.active = 1;
        
        player2.active = 0; // No se usa
        
        board[player1.y][player1.x] = PLAYER1_ID;
        
    } else {
        // MODO MULTIPLAYER: Dos motos en extremos opuestos
        player1.x = BOARD_WIDTH / 4;
        player1.y = BOARD_HEIGHT / 2;
        player1.direction = 'R';
        player1.active = 1;

        player2.x = 3 * BOARD_WIDTH / 4;
        player2.y = BOARD_HEIGHT / 2;
        player2.direction = 'L';
        player2.active = 1;
        
        board[player1.y][player1.x] = PLAYER1_ID;
        board[player2.y][player2.x] = PLAYER2_ID;
    }

    game_running = 1;
    winner = 0;
}

//loop principal
void game_loop() {
    uint64_t last_update = sys_ticks();
    const uint64_t update_interval = 30;
    draw_board(board);
    draw_game_info(mode_select, get_score());
    draw_controls_help(mode_select);
    
    while (game_running) {
        process_input();
        
        uint64_t current_time = sys_ticks();
        if (current_time - last_update >= update_interval) {
            update_game();
           // Solo redibujar si el juego sigue activo
            if (game_running) {
                draw_board(board);
                draw_game_info(mode_select, get_score());
                draw_controls_help(mode_select);
            } 
            
            last_update = current_time;
        }
        
        sys_sleep(10);
    }
    
    show_game_over_screen(mode_select, winner, get_score());
    
    // Esperar Enter para salir
    char c;
    do {
        c = getchar();
    } while (c != '\n');
}

void process_input() {
    // Controles del jugador 1 (WASD)
    if (sys_key_status('w') || sys_key_status('W')) {
        change_direction(&player1, 'U');
    }
    if (sys_key_status('s') || sys_key_status('S')) {
        change_direction(&player1, 'D');
    }
    if (sys_key_status('a') || sys_key_status('A')) {
        change_direction(&player1, 'L');
    }
    if (sys_key_status('d') || sys_key_status('D')) {
        change_direction(&player1, 'R');
    }
    
    // Controles del jugador 2 (IJKL) - solo en multiplayer
    if (mode_select == 2) {
        if (sys_key_status('i') || sys_key_status('I')) {
            change_direction(&player2, 'U');
        }
        if (sys_key_status('k') || sys_key_status('K')) {
            change_direction(&player2, 'D');
        }
        if (sys_key_status('j') || sys_key_status('J')) {
            change_direction(&player2, 'L');
        }
        if (sys_key_status('l') || sys_key_status('L')) {
            change_direction(&player2, 'R');
        }
    }
    
    // Salir del juego
    char c = getchar_nonblock();
    if (c == '\n' || c == 27) {
        game_running = 0;
    }
}

// =================== LÓGICA DEL JUEGO ===================
void update_game() {
    if (!game_running) return;
    
    if (mode_select == 1) {
        // MODO SINGLEPLAYER
        move_moto(&player1, PLAYER1_ID);
        
        if (!player1.active) {
            winner = 0; // Perdió
            game_running = 0;
        }
    } else {
        // MODO MULTIPLAYER
        move_moto(&player1, PLAYER1_ID);
        move_moto(&player2, PLAYER2_ID);
        
        if (!player1.active && !player2.active) {
            winner = 3; // Empate
            game_running = 0;
        } else if (!player1.active) {
            winner = 2; // Jugador 2 gana
            game_running = 0;
        } else if (!player2.active) {
            winner = 1; // Jugador 1 gana
            game_running = 0;
        }
    }
}

void move_moto(Moto *moto, int player_id) {
    if (!moto->active || !game_running) return;
    
    // Calcular nueva posición
    int new_x = moto->x;
    int new_y = moto->y;
    
    switch (moto->direction) {
        case 'U': new_y--; break;
        case 'D': new_y++; break;
        case 'L': new_x--; break;
        case 'R': new_x++; break;
    }
    
    // Verificar colisiones
    if (check_collision_at(new_x, new_y)) {
        moto->active = 0;
        return;
    }
    
    moto->x = new_x;
    moto->y = new_y;
    
    // Marcar rastro en el tablero
    board[moto->y][moto->x] = player_id;
}

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

int check_collision_at(int x, int y) {
    // Colisión con bordes
    if (x < 0 || x >= BOARD_WIDTH || y < 0 || y >= BOARD_HEIGHT) {
        return 1;
    }
    
    // Colisión con rastros
    if (board[y][x] != 0) {
        return 1;
    }
    
    return 0;
}

int get_score() {
    if (mode_select != 1) return 0;
    
    int score = 0;
    for (int i = 0; i < BOARD_HEIGHT; i++) {
        for (int j = 0; j < BOARD_WIDTH; j++) {
            if (board[i][j] == PLAYER1_ID) {
                score++;
            }
        }
    }
    return score;
}

int get_board_value(int x, int y) {
    if (x < 0 || x >= BOARD_WIDTH || y < 0 || y >= BOARD_HEIGHT) {
        return -1;
    }
    return board[y][x];
}


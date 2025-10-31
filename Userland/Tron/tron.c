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

static uint32_t screen_width=0, screen_height=0;

static Moto player1;
static Moto player2;


void tron_main(){
    game_setup();
    int num= starting_screen();
    if(num==0){
        return; //salir del juego
    }   
    mode_select=num;


    initialize_game(&player1, &player2);
    
    
    game_loop();


}

void game_setup(){
    init_graphics();
    screen_width = get_width();
    screen_height = get_height();
    
    // Inicializar jugador 1
    player1.x = BOARD_WIDTH / 4;
    player1.y = BOARD_HEIGHT / 2;
    player1.direction = 'R'; // Derecha
    player1.active = 1;
    player1.draw = draw_moto;

     // Inicializar jugador 2
    player2.x = 3 * BOARD_WIDTH / 4;
    player2.y = BOARD_HEIGHT / 2;
    player2.direction = 'L'; // Izquierda
    player2.active = 1;
    player2.draw = draw_moto;

}

void initialize_game() {
    // Limpiar el tablero
    for (int i = 0; i < BOARD_HEIGHT; i++) {
        for (int j = 0; j < BOARD_WIDTH; j++) {
            board[i][j] = 0; // Celda vacía
        }
    }

    if (mode_select == 1) {
        // MODO SINGLEPLAYER: Una moto en el centro
        player1.x = BOARD_WIDTH / 2;
        player1.y = BOARD_HEIGHT / 2;
        player1.direction = 'R';
        player1.active = 1;
        player1.draw = draw_moto;
        
        // Jugador 2 no se usa
        player2.active = 0;
        
        // Marcar posición inicial
        board[player1.y][player1.x] = PLAYER1_ID;
        
    } else {
        // MODO MULTIPLAYER: Dos motos en extremos opuestos
        player1.x = BOARD_WIDTH / 4;
        player1.y = BOARD_HEIGHT / 2;
        player1.direction = 'R';
        player1.active = 1;
        player1.draw = draw_moto;

        player2.x = 3 * BOARD_WIDTH / 4;
        player2.y = BOARD_HEIGHT / 2;
        player2.direction = 'L';
        player2.active = 1;
        player2.draw = draw_moto;
        
        // Marcar posiciones iniciales
        board[player1.y][player1.x] = PLAYER1_ID;
        board[player2.y][player2.x] = PLAYER2_ID;
    }

    game_running = 1;
    winner = 0;
}
void game_loop() {
    uint64_t last_update = sys_ticks();
    const uint64_t update_interval = 5; // Actualizar cada 5 ticks
    
    while (game_running) {
        // Procesar controles
        process_input();
        
        // Actualizar el juego a intervalos regulares
        uint64_t current_time = sys_ticks();
        if (current_time - last_update >= update_interval) {
            update_game();
            draw_board();
            last_update = current_time;
        }
        
        sys_sleep(20); // Pausa pequeña
    }
    
    // Mostrar resultado final
    show_game_over();
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
    
    // Controles del jugador 2 (IJKL) 
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
    if (c == '\n' || c == 27) { // Enter o ESC
        game_running = 0;
    }
}
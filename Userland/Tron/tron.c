#include "tron.h"
#include "../Userlib/userlib.h"
#include "graphics.h"
#include "logica.h"
#include "levels.h"
#include "config.h"

void tron_main() {
    init_graphics();
    
    int num = starting_screen();
    if (num == 0) {
        return; // Salir del juego
    }   
    
    initialize_game(num);
    game_loop();
}

void game_loop() {
    uint64_t last_update = sys_ticks();
    const uint64_t update_interval = 2;
    
    // Dibujo inicial del tablero completo
    draw_board(get_board());
    draw_game_info(get_mode_select(), get_score());
    draw_level_info(); // Mostrar información del nivel
    draw_controls_help(get_mode_select());
    
    while (get_game_running()) {
        process_input();
        
        uint64_t current_time = sys_ticks();
        if (current_time - last_update >= update_interval) {
            // Actualizar estado del juego
            int game_status = update_game();
            
            if (game_status == 0) {
                // Juego terminado
                break;
            } else if (game_status == 2) {
                // Ronda reiniciada, redibujar todo
                draw_board(get_board());
            
            } else if (game_status == 1 && get_game_running()) {
                // Continúa el juego, redibujar incrementalmente
                if (get_mode_select() == 1) {
                    update_square(get_player1(), PLAYER1_COLOR);
                } else {
                    update_square(get_player1(), PLAYER1_COLOR);
                    update_square(get_player2(), PLAYER2_COLOR);
                }
                
            }
            draw_game_info(get_mode_select(), get_score());
            draw_level_info(); // Mostrar información del nivel
            draw_controls_help(get_mode_select());
            
            last_update = current_time;
        }
        
        sys_sleep(2);
    }
    
    // Mostrar pantalla de game over
    // En singleplayer, usar total_score + level_score actual (puntaje acumulado de todos los niveles)
    // En multiplayer, usar get_score() normal
    int final_score;
    if (get_mode_select() == 1) {
        // En singleplayer, sumar el puntaje del nivel actual al total
        final_score = get_total_score() + get_level_score();
    } else {
        final_score = get_score();
    }
    show_game_over_screen(get_mode_select(), get_winner(), final_score);
    
    // Esperar Enter para salir
    char c;
    do {
        c = getchar();
    } while (c != '\n');
}

void process_input() {
    // Controles del jugador 1 (WASD)
    if (sys_key_status('w') || sys_key_status('W')) {
        process_player_input(PLAYER1_ID, 'w');
    }
    if (sys_key_status('s') || sys_key_status('S')) {
        process_player_input(PLAYER1_ID, 's');
    }
    if (sys_key_status('a') || sys_key_status('A')) {
        process_player_input(PLAYER1_ID, 'a');
    }
    if (sys_key_status('d') || sys_key_status('D')) {
        process_player_input(PLAYER1_ID, 'd');
    }
    
    // Controles del jugador 2 (IJKL) - solo en multiplayer
    if (get_mode_select() == 2) {
        if (sys_key_status('i') || sys_key_status('I')) {
            process_player_input(PLAYER2_ID, 'i');
        }
        if (sys_key_status('k') || sys_key_status('K')) {
            process_player_input(PLAYER2_ID, 'k');
        }
        if (sys_key_status('j') || sys_key_status('J')) {
            process_player_input(PLAYER2_ID, 'j');
        }
        if (sys_key_status('l') || sys_key_status('L')) {
            process_player_input(PLAYER2_ID, 'l');
        }
    }
    
    // Salir del juego
    char c = getchar_nonblock();
    if (c == '\n' || c == 27) {
        set_game_running(0);
    }
}
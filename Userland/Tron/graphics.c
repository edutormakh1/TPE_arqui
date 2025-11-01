#include "graphics.h"
#include "../Userlib/userlib.h"
#include "config.h"

static uint32_t screen_width = 0;
static uint32_t screen_height = 0;
// HUD state to avoid overdraw artifacts
static int hud_last_score = -1;


void init_graphics() {
    sys_screen_size(&screen_width, &screen_height);
    hud_last_score = -1; // reset HUD cache
}

uint32_t get_width() {
    return screen_width;
}

uint32_t get_height() {
    return screen_height;
}

int starting_screen() {
    char num_players;
    
    fill_rectangle(0, 0, screen_width, screen_height, BACKGROUND_COLOR);
    uint32_t center_x = screen_width / 2;
    uint32_t center_y = screen_height / 2;
    draw_string("TRON GAME", center_x - 144, center_y - 100, 4, TEXT_COLOR);
    draw_string("Select number of players:", center_x - 200, center_y - 40, 2, TEXT_COLOR);
    draw_string("Press 1 for Single Player (WASD)", center_x - 280, center_y , 2, TEXT_COLOR);
    draw_string("Press 2 for Two Players (WASD + IJKL)", center_x - 280, center_y + 40, 2, TEXT_COLOR);
    draw_string("Press Enter to quit the game", center_x - 116, center_y + 100, 1, 0x888888);

    do {
        num_players = getchar();
    } while (num_players != '1' && num_players != '2' && num_players != '\n');
    
    if (num_players == '\n') {
        return 0;
    }
    return num_players - '0';
}

void show_game_over_screen(int mode_select, int winner, int score) {
    sys_sleep(100); // Pausa
    
    if (mode_select == 1) {
        // Singleplayer: mostrar puntaje
        draw_string("GAME OVER", screen_width/2 - 144, screen_height/2 - 100, 4, 0xFF0000);
        
        char score_text[32];
        char score_str[16];
        num_to_str(score, score_str, 10);
        
        // Construir mensaje de puntaje
        char* ptr = score_text;
        char* msg = "Score: ";
        while (*msg) *ptr++ = *msg++;
        char* score_ptr = score_str;
        while (*score_ptr) *ptr++ = *score_ptr++;
        *ptr = '\0';
        
        draw_string(score_text, screen_width/2 - 80, screen_height/2, 2, 0xFFFF00);
        
    } else {
        // Multiplayer: mostrar ganador
        if (winner == 1) {
            draw_string("PLAYER 1 WINS!", screen_width/2 - 180, screen_height/2, 3, PLAYER1_COLOR);
        } else if (winner == 2) {
            draw_string("PLAYER 2 WINS!", screen_width/2 - 180, screen_height/2, 3, PLAYER2_COLOR);
        } else if (winner == 3) {
            draw_string("DRAW!", screen_width/2 - 50, screen_height/2, 3, 0xFFFFFF);
        }
    }
    
    draw_string("Press Enter to return to shell", screen_width/2 - 120, screen_height/2 + 80, 1, 0xFFFFFF);
}

// =================== DIBUJO DEL TABLERO ===================
void clear_screen() {
    fill_rectangle(0, 0, screen_width, screen_height, BACKGROUND_COLOR);
}

void draw_board(int board[BOARD_HEIGHT][BOARD_WIDTH]) {
    clear_screen();
    
    // Calcular tamaño de celda
    uint32_t cell_width = screen_width / BOARD_WIDTH;
    uint32_t cell_height = screen_height / BOARD_HEIGHT;
    uint32_t cell_size = (cell_width < cell_height) ? cell_width : cell_height;
    
    // Offset para centrar el tablero
    uint32_t offset_x = (screen_width - (BOARD_WIDTH * cell_size)) / 2;
    uint32_t offset_y = (screen_height - (BOARD_HEIGHT * cell_size)) / 2;
    
    fill_rectangle(offset_x, offset_y, 
                  offset_x + BOARD_WIDTH * cell_size, 
                  offset_y + BOARD_HEIGHT * cell_size, 
                  BACKGROUND_COLOR);

    // Dibujar contenido del tablero
    for (int y = 0; y < BOARD_HEIGHT; y++) {
        for (int x = 0; x < BOARD_WIDTH; x++) {
            int cell_value = board[y][x];
            if (cell_value != 0) {
                draw_moto_cell(x, y, cell_value, cell_size, offset_x, offset_y);
            }
        }
    }
    
    // Dibujar bordes del tablero
    draw_board_border(offset_x, offset_y, cell_size);
}

void draw_moto_cell(int x, int y, int player_id, uint32_t cell_size, uint32_t offset_x, uint32_t offset_y) {
    uint32_t pixel_x = offset_x + x * cell_size;
    uint32_t pixel_y = offset_y + y * cell_size;
    
    uint32_t color = (player_id == PLAYER1_ID) ? PLAYER1_COLOR : PLAYER2_COLOR;

    // Dibujar celda sin margen
    fill_rectangle(pixel_x, pixel_y, 
     pixel_x + cell_size, pixel_y + cell_size, color);
}

void draw_board_border(uint32_t offset_x, uint32_t offset_y, uint32_t cell_size) {
    uint32_t board_x1 = offset_x - 2;
    uint32_t board_y1 = offset_y - 2;
    uint32_t board_x2 = offset_x + BOARD_WIDTH * cell_size + 2;
    uint32_t board_y2 = offset_y + BOARD_HEIGHT * cell_size + 2;
    
    // Dibujar borde más grueso
    draw_rectangle(board_x1, board_y1, board_x2, board_y2, GRID_COLOR);
    draw_rectangle(board_x1 + 1, board_y1 + 1, board_x2 - 1, board_y2 - 1, GRID_COLOR);
}

void draw_game_info(int mode_select, int score) {
    if (mode_select == 1) {
        // Mostrar puntaje en modo singleplayer
        if (score != hud_last_score) {
            // Limpiar banda del HUD de score antes de redibujar para evitar sobreescrituras
            uint32_t x = 8;
            uint32_t y = 8;
            uint32_t size = 2; // mismo size que el texto
            // Limpiamos un ancho generoso para cubrir cambios de dígitos (p.ej., 9 -> 10)
            uint32_t clear_w = screen_width / 4; // ~25% del ancho
            uint32_t clear_h = 20 * size;        // altura aprox. del texto
            if (clear_w < 200) clear_w = 200;
            fill_rectangle(x, y, x + clear_w, y + clear_h, BACKGROUND_COLOR);

            // Construir texto y dibujar
            char score_text[32];
            char score_str[16];
            num_to_str(score, score_str, 10);
            
            char* ptr = score_text;
            char* msg = "Score: ";
            while (*msg) *ptr++ = *msg++;
            char* score_ptr = score_str;
            while (*score_ptr) *ptr++ = *score_ptr++;
            *ptr = '\0';
            
            draw_string(score_text, 10, 10, 2, 0xFFFF00);//score amarillo
            hud_last_score = score;
        }
    }
}

void draw_controls_help(int mode_select) {
    uint32_t y_pos = screen_height - 30;
    // Limpiar la franja inferior antes de dibujar la ayuda para evitar sobreescrituras
    fill_rectangle(0, y_pos - 2, screen_width, y_pos + 20, BACKGROUND_COLOR);

    if (mode_select == 1) {
        draw_string("Controls: WASD to move, Enter to exit", 10, y_pos, 1, 0xFFFFFF);
    } else {
        draw_string("P1: WASD | P2: IJKL | Enter to exit", 10, y_pos, 1, 0xFFFFFF);
    }
}

void animate_collision(int x, int y, uint32_t cell_size) {
    uint32_t offset_x = (screen_width - (BOARD_WIDTH * cell_size)) / 2;
    uint32_t offset_y = (screen_height - (BOARD_HEIGHT * cell_size)) / 2;
    // Calcular posición central de la celda en la pantalla
    uint32_t center_x = offset_x + x * cell_size + cell_size / 2;
    uint32_t center_y = offset_y + y * cell_size + cell_size / 2;
    
    // Animación simple: círculos concéntricos rojos
    for (int i = 1; i <= 5; i++){
        draw_circle(center_x, center_y, i * cell_size / 6, 0xFF0000);
        sys_sleep(5);
    }
}

// Dibuja una celda suelta (x,y) con el mismo cálculo de tamaño/offset que draw_board
void draw_cell_xy(int x, int y, uint32_t color) {
    if (x < 0 || x >= BOARD_WIDTH || y < 0 || y >= BOARD_HEIGHT) return;

    uint32_t cell_width = screen_width / BOARD_WIDTH;
    uint32_t cell_height = screen_height / BOARD_HEIGHT;
    uint32_t cell_size = (cell_width < cell_height) ? cell_width : cell_height;
    if (cell_size == 0) cell_size = 1;

    uint32_t offset_x = (screen_width - (BOARD_WIDTH * cell_size)) / 2;
    uint32_t offset_y = (screen_height - (BOARD_HEIGHT * cell_size)) / 2;

    uint32_t px = offset_x + (uint32_t)x * cell_size;
    uint32_t py = offset_y + (uint32_t)y * cell_size;

    fill_rectangle(px, py, px + cell_size, py + cell_size, color);
}
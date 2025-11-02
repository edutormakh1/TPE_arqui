#include "graphics.h"
#include "../Userlib/userlib.h"
#include "config.h"
#include "logica.h"
#include "tron.h"

static uint32_t screen_width = 0;
static uint32_t screen_height = 0;
// HUD state to avoid overdraw artifacts
static int hud_last_score = -1;
static int hud_last_p1_score = -1;
static int hud_last_p2_score = -1;


void init_graphics() {
    sys_screen_size(&screen_width, &screen_height);
    hud_last_score = -1; // reset HUD cache
    hud_last_p1_score = -1;
    hud_last_p2_score = -1;
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
                  BOARD_BACKGROUND_COLOR);

    // Dibujar grid
    draw_grid(offset_x, offset_y, cell_size);

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
    
    //uint32_t color = (player_id == PLAYER1_ID) ? PLAYER1_COLOR : PLAYER2_COLOR;
    uint32_t color;
    if (player_id == DANGER_ZONE_ID) {
        color = DANGER_ZONE_COLOR; // Color naranja para la franja peligrosa
    } else if (player_id == EXTRA_SPEED_ZONE_ID) {
        color = EXTRA_SPEED_ZONE_COLOR; // Color verde para la franja de velocidad extra
    } else if (player_id == PLAYER1_ID) {
        color = PLAYER1_COLOR;
    } else {
        color = PLAYER2_COLOR;
    }

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

// Dibuja el grid del tablero
void draw_grid(uint32_t offset_x, uint32_t offset_y, uint32_t cell_size) {
    uint32_t board_width_pixels = BOARD_WIDTH * cell_size;
    uint32_t board_height_pixels = BOARD_HEIGHT * cell_size;
    
    // Dibujar líneas verticales
    for (int x = 0; x <= BOARD_WIDTH; x++) {
        uint32_t line_x = offset_x + x * cell_size;
        uint32_t line_y_start = offset_y;
        uint32_t line_y_end = offset_y + board_height_pixels;
        draw_line(line_x, line_y_start, line_x, line_y_end, GRID_LINE_COLOR);
    }
    
    // Dibujar líneas horizontales
    for (int y = 0; y <= BOARD_HEIGHT; y++) {
        uint32_t line_y = offset_y + y * cell_size;
        uint32_t line_x_start = offset_x;
        uint32_t line_x_end = offset_x + board_width_pixels;
        draw_line(line_x_start, line_y, line_x_end, line_y, GRID_LINE_COLOR);
    }
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
    } else if (mode_select == 2) {
        // Mostrar puntaje en modo multiplayer
        int p1_score = get_player1_score();
        int p2_score = get_player2_score();
        
        // Solo redibujar si los puntajes cambiaron
        if (p1_score != hud_last_p1_score || p2_score != hud_last_p2_score) {
            // Limpiar área izquierda (Player 1)
            uint32_t left_x = 8;
            uint32_t left_y = 8;
            uint32_t clear_w = screen_width / 3;
            uint32_t clear_h = 40;
            fill_rectangle(left_x, left_y, left_x + clear_w, left_y + clear_h, BACKGROUND_COLOR);
            
            // Limpiar área derecha (Player 2)
            uint32_t right_x = screen_width - (screen_width / 3) - 8;
            uint32_t right_y = 8;
            fill_rectangle(right_x, right_y, screen_width, right_y + clear_h, BACKGROUND_COLOR);
            
            // Construir texto Player 1 (izquierda)
            char p1_text[64];
            char p1_score_str[16];
            num_to_str(p1_score, p1_score_str, 10);
            
            char* ptr = p1_text;
            char* msg1 = "Player 1: ";
            while (*msg1) *ptr++ = *msg1++;
            char* score1_ptr = p1_score_str;
            while (*score1_ptr) *ptr++ = *score1_ptr++;
            *ptr = '\0';
            
            draw_string(p1_text, left_x, left_y, 2, TEXT_COLOR);
            
            // Construir texto Player 2 (derecha: puntaje primero, luego "Player 2")
            char p2_text[64];
            char p2_score_str[16];
            num_to_str(p2_score, p2_score_str, 10);
            
            ptr = p2_text;
            char* score2_ptr = p2_score_str;
            while (*score2_ptr) *ptr++ = *score2_ptr++;
            char* msg2 = " Player 2";
            while (*msg2) *ptr++ = *msg2++;
            *ptr = '\0';
            
            // Calcular posición X para alinear a la derecha
            // Estimación del ancho del texto (aproximado)
            uint32_t text_estimate_width = 200; // aproximación
            uint32_t right_text_x = screen_width - text_estimate_width - 8;
            draw_string(p2_text, right_text_x, right_y, 2, TEXT_COLOR);
            
            hud_last_p1_score = p1_score;
            hud_last_p2_score = p2_score;
        }
    }
}

void draw_level_info() {
    // Solo mostrar en modo singleplayer
    if (get_mode_select() != 1) return;
    
    int current_level = get_current_level();
    
    // Posición: debajo del tablero, arriba de los controles con más espacio
    uint32_t y_pos = screen_height - 70; // Más espacio arriba de los controles
    
    // Limpiar área antes de dibujar (área más grande para evitar cortes)
    fill_rectangle(0, y_pos - 10, screen_width, y_pos + 40, BACKGROUND_COLOR);
    
    // Construir texto "Nivel x en progreso..."
    char level_text[64];
    char level_str[16];
    num_to_str(current_level, level_str, 10);
    
    char* ptr = level_text;
    char* msg1 = "Nivel ";
    while (*msg1) *ptr++ = *msg1++;
    char* level_ptr = level_str;
    while (*level_ptr) *ptr++ = *level_ptr++;
    char* msg2 = " en progreso...";
    while (*msg2) *ptr++ = *msg2++;
    *ptr = '\0';
    
    // Centrar el texto
    uint32_t text_x = (screen_width / 2) - 150; // Aproximación del ancho del texto
    draw_string(level_text, text_x, y_pos, 2, TEXT_COLOR);
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

// Dibuja de forma incremental la celda actual de la moto en el tablero
void update_square(Moto *moto, uint32_t color) {
    if (moto == 0 || !moto->active) return;

    uint32_t screen_w = get_width();
    uint32_t screen_h = get_height();
    if (screen_w == 0 || screen_h == 0) {
        sys_screen_size(&screen_w, &screen_h);
    }

    uint32_t cell_w = screen_w / BOARD_WIDTH;
    uint32_t cell_h = screen_h / BOARD_HEIGHT;
    uint32_t cell_size = (cell_w < cell_h) ? cell_w : cell_h;
    if (cell_size == 0) cell_size = 1;

    uint32_t offset_x = (screen_w - (BOARD_WIDTH * cell_size)) / 2;
    uint32_t offset_y = (screen_h - (BOARD_HEIGHT * cell_size)) / 2;

    if (moto->x < 0 || moto->x >= BOARD_WIDTH || moto->y < 0 || moto->y >= BOARD_HEIGHT)
        return;

    uint32_t px = offset_x + (uint32_t)moto->x * cell_size;
    uint32_t py = offset_y + (uint32_t)moto->y * cell_size;
    fill_rectangle(px, py, px + cell_size, py + cell_size, color);
}
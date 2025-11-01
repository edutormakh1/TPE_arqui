#include "logica.h"
#include "graphics.h"
#include "config.h"
#include "../Userlib/userlib.h"

void update_square(Moto *moto, uint32_t color) {
    if (moto == 0 || !moto->active) return;

    // Obtener dimensiones de pantalla (usamos caché de graphics, con fallback)
    uint32_t screen_w = get_width();
    uint32_t screen_h = get_height();
    if (screen_w == 0 || screen_h == 0) {
        sys_screen_size(&screen_w, &screen_h);
    }

    // Calcular tamaño de celda y offsets igual que en draw_board
    uint32_t cell_w = screen_w / BOARD_WIDTH;
    uint32_t cell_h = screen_h / BOARD_HEIGHT;
    uint32_t cell_size = (cell_w < cell_h) ? cell_w : cell_h;
    if (cell_size == 0) cell_size = 1; // garantizar al menos 1px por celda

    uint32_t offset_x = (screen_w - (BOARD_WIDTH * cell_size)) / 2;
    uint32_t offset_y = (screen_h - (BOARD_HEIGHT * cell_size)) / 2;

    // Validar límites del tablero
    if (moto->x < 0 || moto->x >= BOARD_WIDTH || moto->y < 0 || moto->y >= BOARD_HEIGHT)
        return;

    // Posición en píxeles y dibujar la celda ocupada por la moto
    uint32_t px = offset_x + (uint32_t)moto->x * cell_size ;
    uint32_t py = offset_y + (uint32_t)moto->y * cell_size;
    fill_rectangle(px, py, px + cell_size, py + cell_size, color);
}

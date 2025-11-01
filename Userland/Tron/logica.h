#ifndef LOGICA_H
#define LOGICA_H

#include <stdint.h>
#include "tron.h"

// Dibuja de forma incremental la celda actual de la moto en el tablero
// usando el color brindado. Calcula tamaño de celda y offsets para
// mantener el tablero centrado, consistente con draw_board.
void update_square(Moto *moto, uint32_t color);

#endif

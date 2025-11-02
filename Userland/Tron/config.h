#ifndef CONFIG_H
#define CONFIG_H

//ID de jugadores
#define PLAYER1_ID 1
#define PLAYER2_ID 2
#define DANGER_ZONE_ID 3 // Zona peligrosa (franja del nivel 2)
#define EXTRA_SPEED_ZONE_ID 4 // Zona de velocidad extra (franja del nivel 3)

//dimension del tablero
#define BOARD_WIDTH 80
#define BOARD_HEIGHT 50

#define CELL_SIZE 10 // Cada celda ocupa 10x10 pixeles

// Sistema de puntos para multiplayer
#define WINNING_POINTS 3 // Puntos necesarios para ganar

// Sistema de niveles para singleplayer
#define MAX_LEVELS 3           // Número total de niveles
#define POINTS_PER_LEVEL 100   // Puntos necesarios para pasar al siguiente nivel




#endif
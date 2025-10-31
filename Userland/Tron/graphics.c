#include "graphics.h"


uint32_t width, height;

void init_graphics() {
    sys_screen_size(&width, &height);
}
uint32_t get_width() {
    return width;
}
uint32_t get_height() {
    return height;
}


int starting_screen(){
    int num_players;
    fillRectangle(0, 0, width, height, BACKGROUND_COLOR);
        draw_string("GOLF GAME", width/2 - 144, height/3, 4, TEXT_COLOR);
    draw_string("Select number of players:", width/2 - 200, height/2, 2,   TEXT_COLOR);
    draw_string("Press 1 for Single Player (WAD)", width/2 - (30 * 8 * 2 / 2), height/2 + 40, 2,   TEXT_COLOR);
    draw_string("Press 2 for Two Players (WAD + IJL)", width/2 - (33 * 8 * 2 / 2), height/3 + 70, 2,  TEXT_COLOR);
    draw_string("Press Enter to quit the game at any time", width/2 - 168, height/3 + 120, 1, 0x888888); // Gray color for less emphasis

    do{
        num_players = getchar();
    } while (num_players != '1' && num_players != '2' && num_players != '\n');
    if(num_players == '\n'){
        return 0;
    }
    return num_players - '0'; //devuelve como entero
}


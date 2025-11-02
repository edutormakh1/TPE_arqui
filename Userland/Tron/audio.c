#include "audio.h"
#include "../Userlib/userlib.h"

// Notas musicales para efectos de sonido (frecuencias en Hz)
#define N_DO2   65    // Do2 - muy muy grave
#define N_DO3   131   // Do3 - muy grave
#define N_MI3   165   // Mi3
#define N_SOL3  196   // Sol3
#define N_DO4   262   // Do4
#define N_MI4   330   // Mi4
#define N_SOL4  392   // Sol4
#define N_DO5   523   // Do5 - agudo

// Reproduce el sonido de colisión/muerte del jugador
// Sonido: rápido, corto, de agudo a grave (descendente)
void play_collision_sound(void) {
    sys_beep(N_DO5, 40);   // Do5 - nota aguda inicial
    sys_beep(N_MI4, 40);   // Mi4 - bajando
    sys_beep(N_SOL3, 40);  // Sol3 - bajando
    sys_beep(N_MI3, 40);   // Mi3 - bajando
    sys_beep(N_DO2, 80);   // Do2 - nota final MUY grave y más larga
}


#include <time.h>

static unsigned long ticks = 0;

void timer_handler() {
	ticks++;
}

int ticks_elapsed() {
	return ticks;
}

int seconds_elapsed() {
	return ticks / 18;
}

// Sleep en milisegundos (aproximado, basado en timer tick cada ~18ms)
void sleep(uint64_t milliseconds) {
	unsigned long start = ticks;
	unsigned long end = start + (milliseconds / 18) + 1; // Convertir ms a ticks (aproximado)
	
	while (ticks < end) {
		// Esperar
	}
}
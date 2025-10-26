#include "include/time.h"
#include "include/video_driver.h"

extern uint8_t getHour();
extern uint8_t getMinutes();
extern uint8_t getSeconds();
uint8_t getDayOfMonth();
uint8_t getMonth();
uint8_t getYear();

static uint64_t ticks = 0;

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
void sleep(int miliseconds) {
	unsigned long start = ticks;
	unsigned long end = start + (miliseconds / 18) + 1; // Convertir ms a ticks (aproximado)
	
	while (ticks < end) {
		// Esperar
	}
}
void get_date(uint8_t *buffer) {
	buffer[0] = getDayOfMonth();
	buffer[1] = getMonth();
	buffer[2] = getYear();
}
void get_time(uint8_t *buffer) {
	buffer[0] = getHour();
	buffer[1] = getMinutes();
	buffer[2] = getSeconds();
}
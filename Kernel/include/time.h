#ifndef _TIME_H_
#define _TIME_H_

#include <stdint.h>

void timer_handler();
int ticks_elapsed();
int seconds_elapsed();
void sleep(int miliseconds);
void get_date(uint8_t *buffer);
void get_time(uint8_t *buffer);

#endif
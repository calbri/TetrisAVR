/*
 * timer0.h
 *
 * Author: Benedict Gattas
 *
 * We set up timer 1 to give us an interrupt
 * every ten milliseconds. 
 */

#ifndef TIMER2_H_
#define TIMER2_H_

#include <stdint.h>

void init_timer2(void);

void set_row_count(uint8_t row_count);

uint8_t get_row_count(void);

#endif
/*
 * timer0.h
 *
 * Author: Benedict Gattas
 *
 * We set up timer 1 to give us an interrupt
 * every ten milliseconds. 
 */

#ifndef TIMER1_H_
#define TIMER1_H_

#include <stdint.h>

/* Set up our timer to give us an interrupt every millisecond
 * and update our time reference. Note: interrupts will need 
 * to be enabled globally for this to work.
 */
void init_timer1(void);

void set_row_count(uint8_t row_count);
#endif
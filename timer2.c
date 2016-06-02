/*
 * timer1.c
 *
 * Author: Benedict Gattas
 *
 * A new timer to interrupt every 10ms, that always runs (even when game is paused).
 * Its sole purpose is to keep the seven_segment display oscillating fast enough 
* to display a two-digit number (how many rows have been completed).
 */

#include <avr/io.h>
#include <avr/interrupt.h>

#include "timer2.h"

/* Our internal clock tick count - incremented every 
 * millisecond. Will overflow every ~49 days. */
static volatile uint32_t clock1_ticks;

/* The (mod 100) count of how many rows have been completed
 */

static volatile uint8_t number_of_rows;
static volatile uint8_t number_to_display;

/* Seven segment display digit being displayed.
** 0 = right digit; 1 = left digit.
*/
volatile uint8_t seven_seg_cc = 0;

uint8_t seven_seg_data[10] = {63,6,91,79,102,109,125,7,127,111};

/* Set up timer 1 to generate an interrupt every 4ms (250 times a second). 
 * We will divide the clock by 256 and count up to 124.
 * We will therefore get an interrupt every 8 x 10000
 * clock cycles, i.e. every 4 milliseconds with an 8MHz
 * clock. 
 * The counter will be reset to 0 when it reaches it's
 * output compare value.
 */
void init_timer2(void) {
	//set the initial number of completed rows to 0
	set_row_count(0);
	/* Make all bits of port C and the least significant
	** bit of port A be output bits.
	*/
	DDRC = 0xFF;
	DDRA |= 0x01;
	
	/* Reset clock tick count. L indicates a long (32 bit) 
	 * constant. 
	 */
	clock1_ticks = 0L;
	
	/* Clear the timer */
	TCNT2 = 0;

	/* Set the output compare value to be 124 */
	OCR2A = 124;
	
	/* Set the timer to clear on compare match (CTC mode)
	 * and to divide the clock by 256. This starts the timer
	 * running.
	 */
	TCCR2A = (1<<WGM21);
	TCCR2B = (1<<CS22);

	/* Enable an interrupt on output compare match. 
	 * Note that interrupts have to be enabled globally
	 * before the interrupts will fire.
	 */
	TIMSK2 |= (1<<OCIE2A);
	
	/* Make sure the interrupt flag is cleared by writing a 
	 * 1 to it.
	 */
	TIFR2 &= (1<<OCF2A);
}

/* Interrupt handler which fires when timer/counter 0 reaches 
 * the defined output compare value (every millisecond)
 */
ISR(TIMER2_COMPA_vect) {
	/* Increment our clock tick count */
	clock1_ticks++;
	
	/* Change which digit will be displayed. If last time was
	** left, now display right. If last time was right, now 
	** display left.
	*/
	seven_seg_cc = 1 ^ seven_seg_cc;
	
	/* Display a digit */
	if(seven_seg_cc == 0) {
		/* Display rightmost digit - seconds */
		PORTC = seven_seg_data[number_to_display%10];
	} else {
		/* Display leftmost digit - ten seconds  */
		PORTC = seven_seg_data[(number_to_display-(number_to_display%10))/10] ;
	}
	/* Output the digit selection (CC) bit */
	PORTA = seven_seg_cc;	
	
}

void set_row_count(uint8_t row_count) {
	//display within 100
	number_of_rows = row_count;
	number_to_display = (number_of_rows % 100);
}

uint8_t get_row_count(void) {
	//return the current row count
	return(number_of_rows);
}

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

#include "timer1.h"

/* Our internal clock tick count - incremented every 
 * millisecond. Will overflow every ~49 days. */
static volatile uint32_t clock1_ticks;

/* The (mod 100) count of how many rows have been completed
 */
static volatile uint8_t number_to_display;

/* Seven segment display digit being displayed.
** 0 = right digit; 1 = left digit.
*/
volatile uint8_t seven_seg_cc = 0;

uint8_t seven_seg_data[10] = {63,6,91,79,102,109,125,7,127,111};

/* Set up timer 1 to generate an interrupt every 10ms (100 times a second). 
 * We will divide the clock by 8 and count up to 9999.
 * We will therefore get an interrupt every 8 x 10000
 * clock cycles, i.e. every 10 milliseconds with an 8MHz
 * clock. 
 * The counter will be reset to 0 when it reaches it's
 * output compare value.
 */
void init_timer1(void) {
	//set the initial number of completed rows to 0
	number_to_display = 0;
	/* Make all bits of port A and the least significant
	** bit of port C be output bits.
	*/
	DDRA = 0xFF;
	DDRC = 0x01;
	
	/* Reset clock tick count. L indicates a long (32 bit) 
	 * constant. 
	 */
	clock1_ticks = 0L;
	
	/* Clear the timer */
	TCNT1 = 0;

	/* Set the output compare value to be 124 */
	OCR1A = 9999;
	
	/* Set the timer to clear on compare match (CTC mode)
	 * and to divide the clock by 8. This starts the timer
	 * running.
	 */
	TCCR1A = (1<<WGM11);
	TCCR1B = (1<<CS11);

	/* Enable an interrupt on output compare match. 
	 * Note that interrupts have to be enabled globally
	 * before the interrupts will fire.
	 */
	TIMSK1 |= (1<<OCIE1A);
	
	/* Make sure the interrupt flag is cleared by writing a 
	 * 1 to it.
	 */
	TIFR1 &= (1<<OCF1A);
}
/*
uint32_t get_clock_ticks(void) {
	uint32_t return_value;

	/* Disable interrupts so we can be sure that the interrupt
	 * doesn't fire when we've copied just a couple of bytes
	 * of the value. Interrupts are re-enabled if they were
	 * enabled at the start.
	 
	uint8_t interrupts_were_on = bit_is_set(SREG, SREG_I);
	cli();
	return_value = clock_ticks;
	if(interrupts_were_on) {
		sei();
	}
	return return_value;
}
*/

/* Interrupt handler which fires when timer/counter 0 reaches 
 * the defined output compare value (every millisecond)
 */
ISR(TIMER1_COMPA_vect) {
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
		PORTA = seven_seg_data[number_to_display%10];
	} else {
		/* Display leftmost digit - ten seconds  */
		PORTA = seven_seg_data[(number_to_display-(number_to_display%10))/10] ;
	}
	/* Output the digit selection (CC) bit */
	PORTC = seven_seg_cc;	
	
}

void set_row_count(uint8_t row_count) {
	//display within 100
	number_to_display = (row_count % 100);
}

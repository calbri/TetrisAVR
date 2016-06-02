/*
 * buttons.c
 *
 * Author: Peter Sutton
 */ 

#include <avr/io.h>
#include <avr/interrupt.h>
#include "buttons.h"

// Global variable to keep track of the last button state so that we 
// can detect changes when an interrupt fires. The lower 4 bits (0 to 3)
// will correspond to the last state of port B pins 0 to 3.
static volatile uint8_t last_button_state;

// Our button queue. button_queue[0] is always the head of the queue. If we
// take something off the queue we just move everything else along. We don't
// use a circular buffer since it is usually expected that the queue is very
// short. In most uses it will never have more than 1 element at a time.
// This button queue can be changed by the interrupt handler below so we should
// turn off interrupts if we're changing the queue outside the handler.
#define BUTTON_QUEUE_SIZE 8
static volatile uint8_t button_queue[BUTTON_QUEUE_SIZE];
static volatile int8_t queue_length;
static volatile uint8_t x_or_y = 0;	/* 0 = x, 1 = y */
static volatile uint8_t most_recent_joystick;
static volatile	uint8_t no_change_in_x, no_change_in_y;

// Setup interrupt if any of pins B0 to B3 change. We do this
// using a pin change interrupt. These pins correspond to pin
// change interrupts PCINT8 to PCINT11 which are covered by
// Pin change interrupt 1.
void init_button_interrupts(void) {
	// Enable the interrupt (see datasheet page 69)
	PCICR |= (1<<PCIE1);
	
	// Make sure the interrupt flag is cleared (by writing a 
	// 1 to it) (see datasheet page 69)
	PCIFR |= (1<<PCIF1);
	
	// Choose which pins we're interested in by setting
	// the relevant bits in the mask register (see datasheet page 70)
	PCMSK1 |= (1<<PCINT8)|(1<<PCINT9)|(1<<PCINT10)|(1<<PCINT11);	
	
	// Empty the button push queue
	queue_length = 0;
}

void empty_button_queue(void) {
	queue_length = 0;
	last_button_state = 0;
}

int8_t button_pushed(void) {
	int8_t return_value = -1;	// Assume no button pushed
	if(queue_length > 0) {
		// Remove the first element off the queue and move all the other
		// entries closer to the front of the queue. We turn off interrupts (if on)
		// before we make any changes to the queue. If interrupts were on
		// we turn them back on when done.
		return_value = button_queue[0];
		

	}
	return return_value;
}

// Interrupt handler for a change on buttons
ISR(PCINT1_vect) {
	// Get the current state of the buttons. We'll compare this with
	// the last state to see what has changed.
	uint8_t button_state = PINB & 0x0F;

	// If we have space in the queue, then iterate over all the buttons
	// and see which ones have changed.	If we have no space in the queue
	// we don't bother - we just ignore the button event.  (Ideally
	// this should never happen.)
	if(queue_length < BUTTON_QUEUE_SIZE) {
		// Iterate over all the buttons and see which ones have changed.
		// Any button pushes are added to the queue of button pushes (if
		// there is space). We ignore button releases so we're just looking
		// for a transition from 0 in the last_button_state bit to a 1 in the
		// button_state.
		for(uint8_t pin=0; pin<=3; pin++) {
			if((button_state & (1<<pin)) &&	!(last_button_state & (1<<pin))) {
				// Add the button push to the queue (and update the
				// length of the queue). If the queue is now full, we stop
				// processing (i.e. ignore other button events if there
				// are any)
				button_queue[queue_length++] = pin;
				if(queue_length >= BUTTON_QUEUE_SIZE) {
					break;
				}
			} else if (!(button_state & (1<<pin)) && (last_button_state & (1<<pin))) {
				queue_length = 0;
			}
		}
	}
		
	// Remember this button state so we can detect changes next time
	last_button_state = button_state;
}

uint8_t joystick_input(void) {
	uint8_t return_value = -1;		//assume no joystick input
	//Set the ADC mux to choose ADC6 if x_or_y is 0, ADC7 is x_or_y is 1
	if(x_or_y == 0) {
		ADMUX |= (1<<2)|(1<<1)|(1<<0);
	} else {
		ADMUX &= ~1;
	}
	//start the conversion
	ADCSRA |= (1<<ADSC);
	
	while(ADCSRA & (1<<ADSC)) {
		; /* Wait until conversion finished */
	}	
	//conversion is over
	
	//determine a value. 0 = right, 1=drop block, 2=rotate block, 3=left
	//check x first, then y
	if (x_or_y == 0) {
		if (ADC > 700) {
			most_recent_joystick = 0;
			no_change_in_x = 0;
		} else if (ADC < 300) {
			most_recent_joystick = 3;
			no_change_in_x = 0;			
		} else {
			//keep most_recent_joystick what it was
			no_change_in_x = 1;
		}
	} else {
		if (ADC > 700) {
			most_recent_joystick = 2;
			no_change_in_y = 0;
		} else if (ADC < 300) {
			most_recent_joystick = 1;
			no_change_in_y = 0;
		} else {
			//keep most_recent_joystick what it was			
			no_change_in_y = 1;
		}
	}
	x_or_y ^= 1;
	
	if(no_change_in_x && no_change_in_y) {
		most_recent_joystick = -1;
	}
	
	return most_recent_joystick;
	
}

uint8_t get_most_recent_joystick(void) {
	return most_recent_joystick;
}


/*
ISR(ADC) {
	//do nothing
}*/
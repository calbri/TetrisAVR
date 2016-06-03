/*
 * timer1.c
 *
 * Author: Benedict Gattas
 *
 * A new timer to handle music. 
 *
 *  For a BPM of x, where BPM is quaver beats per minute, an quaver needs to be generated 
 *  x times a minute, which is x/4 times a second, or every (x/4 * 1000) ms
 *  For example, a BPM of 240 (240 quavers a second) will mean an interrupt 4 times a second,
 *  which means every 250ms. This means every 250ms the note being played needs to be updated
 *  to the next in the array quaverBeats (see global variables).
 *  
 *
 * The buzzer outputs the frequency at which the pin is toggled (assume toggle
 * per interrupt). Interrupt interval in milliseconds must therefore occur according to 
 * a variable amount determined by the note: (note G# is the lowest note in the tetris theme)
_____________________________________________________________________________________________
|LowNote  | Freq.(Hz) | How often interrupts must occur to create the note = 1000ms/frequency |
| G#      |  415.305  | 2.408ms                                                               |
| A       |  440.000  | 2.273ms                                                               |
| B       |  493.883  | 2.025ms                                                               |
| C       |  523.251  | 1.911ms                                                               |
| D       |  587.330  | 1.703ms                                                               |
| E       |  659.255  | 1.517ms                                                               |
| F       |  698.456  | 1.432ms                                                               |
| G       |  783.991  | 1.276ms                                                               |
| G#      |  830.609  | 1.204ms                                                               |
| A       |  880.000  | 1.136ms                                                               |
| silence |  000.000  | ~assume to be 1000ms                                                  |
|HighNote |			  |                                                                       |
``````````````````````````````````````````````````````````````````````````````````````````````

* STORING THE TETRIS THEME IN QUAVERS:
*  -->There are 8 quaver beats in a bar, and 32 bars in the piece, therefore 256 quavers to 
 *		store
 *  -->A crotchet (2 quaver beats) is output as a quaver beat followed by a silent quaver
 *	-->A dotted crotched (3 quaver beats) is output as a quaver beat followed by two silent quavers.
 *  -->A minum (4 quaver beats) is output as a quaver beat followed by 3 of the same quaver
 *  -->Crotchet rests and minum rests are 2 and 4 silent quavers respectively
 *  There are 32 bars in the piece. Together the quavers can be store in an array that's elements
 *	correspond to each note..
 
 *	This is the piece separated by bars: (in quaver beats as described above, where a silent beat = 0)
 *	{(E,0,B,C,D,0,C,B),(A,0,A,C,E,0,D,C),(B,0,0,C,D,0,E,0),(C,0,A,0,A,A,B,C)
	 (D,0,0,F,A(HIGH),0,G,F),(E,0,0,C,E,0,D,C),(B,0,B,C,D,0,E,0),(C,0,A,0,A,0,0,0)
	 (E,E,E,E,C,C,C,C),(D,D,D,D,B,B,B,B),(C,C,C,C,A,A,A,A),(B,B,B,B,G#,G#,G#,G#,)
	 (E,E,E,E,C,C,C,C),(D,D,D,D,B,B,B,B),(C,C,E,E,A(HIGH),A(HIGH),A(HIGH),A(HIGH)),
	 (G#(HIGH),G#(HIGH),G#(HIGH),G#(HIGH),G#(HIGH),G#(HIGH),G#(HIGH),G#(HIGH))}
 *	(with repeats)
 */

#include <avr/io.h>
#include <avr/interrupt.h>

#include "timer1.h"
#include "timer2.h"

//OCRA values, the note "frequency" array (see table above)
//Let 1ms be 1000 in this array
uint32_t note_frequencies[15] = {2408, 2273, 2025, 1911, 1703, 1517, 1432, 1276, 1204, 1136, 1000000,0,0,0,0};

//game over theme
uint32_t new_note_frequencies[15] = {3214,2863,2408,2272,2145,1911,1804,1703,1607,1517,1432,1276,1204,1073,1000000};

//a variable to keep track of how far into the piece the buzzer is
uint32_t numTicks;

//a variable to track whether a piece, or a game buzz tone, is playing
volatile uint8_t game_tone_is_playing, silent_note;

//current note in the piece (0 to 255)
uint8_t current_note, game_over;

//current speed
uint32_t speed_threshold;

//buzzer_toggle
uint8_t toggle;

//the piece, where each element is an index into note_frequencies, ordered by the notes in the piece
//see comment block above
//(values: G#=0,A=1,B=2,C=3,D=4,E=5,F=6,G=7,G#(HIGH)=8,A(HIGH)=9,silence=10)
uint8_t tetris_theme[256] =    {5,10,2,3,4,10,3,2,  1,10,1,3,5,10,4,3,   2,10,10,3,4,10,5,10,  3,10,1,10,1,1,2,3, 
								4,10,10,6,9,10,7,6, 5,10,10,3,5,10,4,3,  2,10,2,3,4,10,5,10,   3,10,1,10,1,10,10,10,
								5,10,2,3,4,10,3,2,  1,10,1,3,5,10,4,3,   2,10,10,3,4,10,5,10,  3,10,1,10,1,1,2,3,
								4,10,10,6,9,10,7,6, 5,10,10,3,5,10,4,3,  2,10,2,3,4,10,5,10,   3,10,1,10,1,10,10,10,
								5,5,5,5,3,3,3,3,    4,4,4,4,2,2,2,2,     3,3,3,3,1,1,1,1,      2,2,2,2,0,0,0,0,
								5,5,5,5,3,3,3,3,    4,4,4,4,2,2,2,2,     3,3,5,5,9,9,9,9,      8,8,8,8,8,8,8,8,
								5,10,2,3,4,10,3,2,  1,10,1,3,5,10,4,3,   2,10,10,3,4,10,5,10,  3,10,1,10,1,1,2,3,
								4,10,10,6,9,10,7,6, 5,10,10,3,5,10,4,3,  2,10,2,3,4,10,5,10,   3,10,1,10,1,10,10,10};

uint8_t new_tetris_theme[256] = {14,14,8,8,10,10,11,11,   12,12,12,13,12,12,12,11,   11,11,11,10,11,11,11,10,	10,10,10,8,10,10,10,8,
		8,8,8,6,5,5,5,8,		 12,12,12,13,12,12,12,11,   11,11,11,10,11,11,11,10,	10,10,10,8,10,10,10,8,
		8,8,8,6,5,5,5,4,		 5,5,5,6,5,5,5,4,			4,4,4,3,4,4,4,5,			6,6,6,8,6,6,6,5,
		5,5,5,4,5,5,5,6,		 8,8,8,7,8,8,8,9,		    11,11,11,10,12,12,12,10,	8,8,8,6,5,5,5,4,
		2,0,1,5,2,2,2,2,		 14,14,14,14,14,14,14,14,	14,14,14,14,14,14,14,14,	14,14,14,14,14,14,14,14,
		14,14,8,8,10,10,11,11,   12,12,12,13,12,12,12,11,   11,11,11,10,11,11,11,10,	10,10,10,8,10,10,10,8,
		8,8,8,6,5,5,5,8,		 12,12,12,13,12,12,12,11,   11,11,11,10,11,11,11,10,	10,10,10,8,10,10,10,8,
		8,8,8,6,5,5,5,4,		 5,5,5,6,5,5,5,4,			4,4,4,3,4,4,4,5,			5,5,4,4,2,2,14,14};


/* Set up timer 0 to generate an interrupt every
 *	 (note_multipliers[current_note_multiplier])/1000 milliseconds. 
 * We will divide the clock by 8 (1MHz). Therefore an output compare value of 999 (1000-1) is 
 *  an interrupt every millisecond.
 * Therefore the output compare value we need is simply the value in (note_frequencies[i]-1)
 *
 * The counter will be reset to 0 when it reaches it's
 * output compare value.
 */
void init_timer1(void) {
	// Make pin A1 be an input (the mute switch) and pin A2 be the buzzer output
	DDRA |= (0<<1)|(1<<2);
	
	//start the piece at note 0 (0 of 255)
	current_note = 0;
	toggle = 0;
	numTicks = 0;
	game_over = 0;
	game_tone_is_playing = 0;
	speed_threshold = 75000;	//125000 is medium pace, 50-60000 is very fast
	silent_note = 10;

	/* Clear the timer */
	TCNT1 = 0;

	// Set the output compare value to be the first note, or an E in the above table
	OCR1A = 0.1*(note_frequencies[tetris_theme[current_note]]-1);
	
	/* Set the timer to clear on compare match (CTC mode)
	 * and to divide the clock by 8. This starts the timer
	 * running.
	 */
	TCCR1A = 0;
	TCCR1B = (1<<WGM12)|(1<<CS11);

	/* Enable an interrupt on output compare match. 
	 * Note that interrupts have to be enabled globally
	 * before the interrupts will fire.
	 */
	TIMSK1 |= (1<<OCIE1A);//|(1<<OCIE1B);
	
	/* Make sure the interrupt flag is cleared by writing a 
	 * 1 to it.
	 */
	TIFR1 &= (1<<OCF1A);
}

/* Interrupt handler which fires when timer/counter 0 reaches 
 * the defined output compare value. Toggles the buzzer
 */
ISR(TIMER1_COMPA_vect) {
	numTicks += OCR1A;
	uint8_t mute = ((PINA & (1<<1)) >> 1);
	if (game_over==0) {
		if (game_tone_is_playing == 0) {
			if (numTicks > (125000)) {
				numTicks = 0;
				if (current_note==255){
					current_note = 0;
					} else {
					current_note++;
				}
				OCR1A = 0.1*(note_frequencies[tetris_theme[current_note]]-1);
			}
			//if the note is a 10 make it silent (see tetris_theme definition)
			if (tetris_theme[current_note]==10) {
				mute = 1;
			}
		} else {
			switch(game_tone_is_playing) {
				case 1 :
					//play a high A
					OCR1A = 0.1*(note_frequencies[9]-1);
				case 2 :
					//play a high F
					OCR1A = 0.1*(note_frequencies[7]-1);
				}
			//play the tone the normal note length above
			if (numTicks > (speed_threshold)) {
				numTicks = 0;
			}
			game_tone_is_playing = 0;
		}
	} else {
		if (numTicks > (200000)) {
			numTicks = 0;
			if (current_note==255){
				current_note = 0;
				} else {
				current_note++;
			}
			OCR1A = 0.1*(new_note_frequencies[new_tetris_theme[current_note]]-1);
		}
		//if the note is a 14 make it silent (see tetris_theme definition)
		if (tetris_theme[current_note]==14) {
			mute = 1;
		}
	}
	toggle = (1 ^ toggle);
	//check if mute is enabled
	if (mute==1) {
		//output a 0
		PORTA &= (11111011);
	} else {
		if (toggle) {
			PORTA |= (1<<2);
		} else {
			PORTA &= (11111011);
		}
	}	
	
}

void play_game_tone(uint8_t tone_number) {
	uint8_t interrupts_were_on = bit_is_set(SREG, SREG_I);
	cli();
	game_tone_is_playing = tone_number;
	if(interrupts_were_on) {
		sei();
	}
}

void switch_to_game_over(void) {
	current_note = 0;
	numTicks = 0;
	game_tone_is_playing = 0;
	game_over = 1;
}
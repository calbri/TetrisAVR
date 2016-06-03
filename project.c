/*
 * project.c
 *
 * Main file for the Tetris Project.
 *
 * Author: Peter Sutton. Modified by Callum Bryson and Benedict Gattas
 */ 

#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/pgmspace.h>
#include <stdio.h>
#include <stdlib.h>		// For random()

#include "ledmatrix.h"
#include "scrolling_char_display.h"
#include "buttons.h"
#include "serialio.h"
#include "terminalio.h"
#include "score.h"
#include "timer0.h"
#include "timer1.h"
#include "timer2.h"
#include "game.h"

#define F_CPU 8000000L
#include <util/delay.h>

// Function prototypes - these are defined below (after main()) in the order
// given here
void initialise_hardware(void);
void splash_screen(void);
void new_game(void);
void play_game(void);
void handle_game_over(void);
void handle_new_lap(void);

// ASCII code for Escape character
#define ESCAPE_CHAR 27

/////////////////////////////// main //////////////////////////////////
int main(void) {
	// Setup hardware and call backs. This will turn on 
	// interrupts.
	initialise_hardware();
	
	//storage
	manage_eeprom();
	
	// Show the splash screen message. Returns when display
	// is complete
	splash_screen();
	
	//initialise high score at 0
	set_high_score(0);
	
	while(1) {
		//seed the random number generator
		//multiply by 10 to get good spread
		empty_button_queue();
		srandom(get_clock_ticks()*10);
		new_game();
		play_game();
		handle_game_over();
	}
}

void initialise_hardware(void) {
	ledmatrix_setup();
	init_button_interrupts();
	
	// Setup serial port for 19200 baud communication with no echo
	// of incoming characters
	init_serial_stdio(19200,0);

	// Set up our main timer to give us an interrupt every millisecond
	init_timer0();
	//play music
	init_timer1();
	//set up the secondary timer to keep the seven_seg_display *always* displaying two digits
	init_timer2();	
	// Turn on global interrupts
	sei();
	
	//set-up the joystick
	ADMUX = (1<<REFS0);
	ADCSRA = (1<<ADEN)|(1<<ADPS2)|(1<<ADPS1);	
	
	
	
}

void splash_screen(void) {
	// Reset display attributes and clear terminal screen then output a message
	set_display_attribute(TERM_RESET);
	clear_terminal();
	
	hide_cursor();	// We don't need to see the cursor when we're just doing output
	move_cursor(3,3);
	printf_P(PSTR("Tetris"));
	
	move_cursor(3,5);
	set_display_attribute(FG_GREEN);	// Make the text green
	printf_P(PSTR("CSSE2010/7201 Tetris Project by Ben Gattas and Callum Bryson"));	
	set_display_attribute(FG_WHITE);	// Return to default colour (White)
	
	move_cursor(17,7);
	printf_P(PSTR("High Scores: "));
	for (uint8_t i = 0; i < 5; i++) {
		move_cursor(17, 8+i);
		printf_P(PSTR("%10d"),get_eeprom_scores()[i]);
		printf_P(PSTR(" "));
		for (uint8_t j =0; j < 3; j++) {
			char initialCharacter = get_eeprom_initial(i)[j];
			printf("%c",initialCharacter);
		}
	}
	
	// Output the scrolling message to the LED matrix
	// and wait for a push button to be pushed.
	ledmatrix_clear();
	
	// Red message the first time through
	PixelColour colour = COLOUR_RED;
	while(1) {
		set_scrolling_display_text("TETRIS 43922604  43915398", colour);
		// Scroll the message until it has scrolled off the 
		// display or a button is pushed. We pause for 130ms between each scroll.
		while(scroll_display()) {
			_delay_ms(130);
			if(button_pushed() != -1) {
				// A button has been pushed
				return;
			}
		}
		// Message has scrolled off the display. Change colour
		// to a random colour and scroll again.
		switch(random()%4) {
			case 0: colour = COLOUR_LIGHT_ORANGE; break;
			case 1: colour = COLOUR_RED; break;
			case 2: colour = COLOUR_YELLOW; break;
			case 3: colour = COLOUR_GREEN; break;
		}
	}
}

void new_game(void) {
	//switch music
	switch_to_game_over(0);
	
	// Initialise the game and display
	init_game();
	
	// Clear the serial terminal
	clear_terminal();
	
	// Initialise the score
	init_score();
	
	//display score
	display_score(get_score());
	
	//display game area
	draw_game_window();
	
	//display initial next block
	initial_display_next_block();
	
	// Delete any pending button pushes or serial input
	empty_button_queue();
	clear_serial_input_buffer();
}

void play_game(void) {
	uint32_t last_drop_time, last_term_time, last_input_time;
	int8_t button, game_paused, last_button, joystick, last_joystick;
	char serial_input, escape_sequence_char;
	uint8_t characters_into_escape_sequence = 0;
	
	int firstRepeat = 0;
	last_button = -2;
	last_joystick = get_most_recent_joystick();
	
	// Record the last time a block was dropped as the current time -
	// this ensures we don't drop a block immediately.
	last_drop_time = get_clock_ticks();
	last_input_time = get_clock_ticks();
	last_term_time = get_clock_ticks();
	
	// We play the game forever. If the game is over, we will break out of
	// this loop. The loop checks for events (button pushes, serial input etc.)
	// and on a regular basis will drop the falling block down by one row.
	while(1) {
		
		//update serial display
		if(get_clock_ticks() > last_term_time + 100) {
			fast_terminal_draw();
			last_term_time = get_clock_ticks();
		}
		
		
		// Check for input - which could be a button push or serial input.
		// Serial input may be part of an escape sequence, e.g. ESC [ D
		// is a left cursor key press. We will be processing each character
		// independently and can't do anything until we get the third character.
		// At most one of the following three variables will be set to a value 
		// other than -1 if input is available.
		// (We don't initalise button to -1 since button_pushed() will return -1
		// if no button pushes are waiting to be returned.)
		// Button pushes take priority over serial input. If there are both then
		// we'll retrieve the serial input the next time through this loop
		serial_input = -1;
		escape_sequence_char = -1;
		button = button_pushed();
		joystick = joystick_input();
			
		//check if last button pressed or joystick input was the same as this one
		if ((last_button == button) && (button != -1)) {
			if (firstRepeat == 0) {
				if (get_clock_ticks() >= last_input_time + 500) {
					if(button==3 || escape_sequence_char=='D') {
						// Attempt to move left
						(void)attempt_move(MOVE_LEFT);
					} else if(button==0 || escape_sequence_char=='C') {
						// Attempt to move right
						(void)attempt_move(MOVE_RIGHT);
					} else if (button==2 || escape_sequence_char == 'A') {
						// Attempt to rotate
						(void)attempt_rotation();
					}
					firstRepeat = 1;
				}
			} else {
				if (get_clock_ticks() >= last_input_time + 50) {
					if(button==3 || escape_sequence_char=='D') {
						// Attempt to move left
						(void)attempt_move(MOVE_LEFT);
					} else if(button==0 || escape_sequence_char=='C') {
						// Attempt to move right
						(void)attempt_move(MOVE_RIGHT);
					} else if (button==2 || escape_sequence_char == 'A') {
						// Attempt to rotate
						(void)attempt_rotation();
					}
				}
			}
			
		} else if ((last_joystick == joystick) && (joystick != -1)){
			if (firstRepeat == 0) {
				if (get_clock_ticks() >= last_input_time + 500) {
					if(joystick==3) {
						// Attempt to move left
						(void)attempt_move(MOVE_LEFT);
					} else if(joystick==0) {
						// Attempt to move right
						(void)attempt_move(MOVE_RIGHT);
					} else if (joystick==2) {
						// Attempt to rotate
						(void)attempt_rotation();
					}
					firstRepeat = 1;
				}
			} else {
				if (get_clock_ticks() >= last_input_time + 50) {
					if (joystick==3) {
						// Attempt to move left
						(void)attempt_move(MOVE_LEFT);
					} else if (joystick==0) {
						// Attempt to move right
						(void)attempt_move(MOVE_RIGHT);
					} else if (joystick==2) {
						// Attempt to rotate
						(void)attempt_rotation();
					}
				}
			}
			
		
		} else {
			last_input_time = get_clock_ticks();
			last_button = button;
			last_joystick = joystick;
			firstRepeat = 0;		
			if(button == -1) {
				// No push button was pushed, see if there is any serial input
				if(serial_input_available()) {
					// Serial data was available - read the data from standard input
					serial_input = fgetc(stdin);
					// Check if the character is part of an escape sequence
					if(characters_into_escape_sequence == 0 && serial_input == ESCAPE_CHAR) {
						// We've hit the first character in an escape sequence (escape)
						characters_into_escape_sequence++;
						serial_input = -1; // Don't further process this character
					} else if(characters_into_escape_sequence == 1 && serial_input == '[') {
						// We've hit the second character in an escape sequence
						characters_into_escape_sequence++;
						serial_input = -1; // Don't further process this character
					} else if(characters_into_escape_sequence == 2) {
						// Third (and last) character in the escape sequence
						escape_sequence_char = serial_input;
						serial_input = -1;  // Don't further process this character - we
											// deal with it as part of the escape sequence
						characters_into_escape_sequence = 0;
					} else {
						// Character was not part of an escape sequence (or we received
						// an invalid second character in the sequence). We'll process 
						// the data in the serial_input variable.
						characters_into_escape_sequence = 0;
					}
				}
			}
			// Process the input. 
			if(button==3 || escape_sequence_char=='D' || joystick==3) {
				// Attempt to move left
				(void)attempt_move(MOVE_LEFT);
			} else if(button==0 || escape_sequence_char=='C' || joystick==0) {
				// Attempt to move right
				(void)attempt_move(MOVE_RIGHT);
			} else if (button==2 || escape_sequence_char == 'A' || joystick==2) {
				// Attempt to rotate
				(void)attempt_rotation();
			} else if (button==1 || escape_sequence_char == 'B' || joystick==1) {
				// Attempt to drop block 
				drop:if(!attempt_drop_block_one_row()) {
						// Drop failed - fix block to board and add new block
						if(!fix_block_to_board_and_add_new_block()) {
							break;	// GAME OVER
						}
				} else {
					add_to_score(1); //block dropped
					//ADDED FUNCTIONALITY - repeat until can no longer be dropped
					goto drop;
					//display score
					display_score(get_score());
				}
				last_drop_time = get_clock_ticks();
			} else if(serial_input == 'p' || serial_input == 'P') {
				// pause/un-pause the game until 'p' or 'P' is pressed again.
				// All other input (buttons, serial etc.) must be ignored. Except new game.
				game_paused = 1;
				//stop the game timer (see timer0.c for implementation of toggle_timer)
				toggle_timer();
				while (game_paused) {
					//wait for only a 'p' or an 'n' to come through
					if (serial_input_available()) {
						serial_input = fgetc(stdin);
						if (serial_input == 'p' || serial_input == 'P') {
							game_paused = 0;
						}
						if (serial_input == 'n' || serial_input == 'N') {
							game_paused = 0;
							new_game();
						}
					}
				}
				//restart the game timer
				toggle_timer();
			} else if(serial_input == 'n' || serial_input == 'N') {
				//reset the game state and begin a new game
				//TO DO LATER: save high-score here
				new_game();
			} else if(serial_input == 's' || serial_input == 'S') {
				//save the game state
				save_game();
			} else if(serial_input == 'o' || serial_input == 'O') {
				load_game();
			}
		}
		// else - invalid input or we're part way through an escape sequence -
		// do nothing
		
		// Check for timer related events here
		uint16_t num_ticks = 600;
		if (get_row_count() < 30) {
			num_ticks = 600 - (get_row_count()*20);
		} else {
			num_ticks = 20;
		}
		
		if(get_clock_ticks() >= last_drop_time + num_ticks) {
			// 600ms (0.6 second) has passed since the last time we dropped
			// a block, so drop it now.
			if(!attempt_drop_block_one_row()) {
				// Drop failed - fix block to board and add new block
				if(!fix_block_to_board_and_add_new_block()) {
					break;	// GAME OVER
				}
			}
			last_drop_time = get_clock_ticks();
		}
	}
	// If we get here the game is over. 
}

void handle_game_over() {
	switch_to_game_over(1);
	empty_button_queue();
	move_cursor(17,14);
	// Print a message to the terminal. 
	printf_P(PSTR("GAME OVER"));
	//output current high score
	if (get_score() > get_high_score()) {
		set_high_score(get_score());
	}
	move_cursor(17,15);
	printf_P(PSTR("HIGH SCORE: %d"), get_high_score());
	uint8_t new_best_score = 0;
	//check for new high score
	uint8_t index;
	for (uint8_t j = 0; j<5; j++) {
		if (get_score() > get_eeprom_scores()[j]) {
			new_best_score = 1;
			index = j;
			break;
		}
	}
	if (new_best_score == 1) {
		//input a new best score
		move_cursor(17,17);
		printf_P(PSTR("Enter initials: "));
		show_cursor();
		static char initials[3];
		uint8_t initial_num = 0;
		uint32_t time_since_wait = get_clock_ticks();
		while (1) {
			if (serial_input_available()) {
				char input = fgetc(stdin);
				initials[initial_num] = input;
				printf("%c", input);
				initial_num++;
			}
			if (initial_num > 2) {
				for (uint8_t j = 4; j > index; j--) {
					store_eeprom_score(get_eeprom_scores()[j-1],j);
					store_eeprom_initials(get_eeprom_initial(j-1),j);
				}
				store_eeprom_initials(initials, index);
				store_eeprom_score(get_score(), index);
				break;
			}
			if (get_clock_ticks() > time_since_wait + 10000) {
				break;
			}
		}
		hide_cursor();
		
	}
	move_cursor(17,18);
	printf_P(PSTR("High Scores: "));
	move_cursor(17,19);
	for (uint8_t i = 0; i < 5; i++) {
		move_cursor(17, 19+i);
		printf_P(PSTR("%10d"),get_eeprom_scores()[i]);
		printf_P(PSTR(" "));
		for (uint8_t j =0; j < 3; j++) {
			char initialCharacter = get_eeprom_initial(i)[j];
			printf("%c",initialCharacter);
		}
	move_cursor(17,17);
	printf_P(PSTR("Press a button to start again"));
		
	}
	while(button_pushed() == -1) {
		if (serial_input_available()) {
			char serial_input = fgetc(stdin);
			if (serial_input == 'n' || serial_input == 'N') {
				break;
			}

		}
		; // wait until a button has been pushed
	}
	
}


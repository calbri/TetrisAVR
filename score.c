/*
 * score.c
 *
 * Written by Peter Sutton
 */

#include "score.h"
#include <avr/eeprom.h>

// Variable to keep track of the score. We declare it as static
// to ensure that it is only visible within this module - other
// modules should call the functions below to modify/access the
// variable.
static uint32_t score;
static uint32_t high_score;

uint32_t loaded_scores[5];
char *loaded_initials[5];

void init_score(void) {
	score = 0;
}

void add_to_score(uint16_t value) {
	score += value;
}

uint32_t get_score(void) {
	return score;
}

void set_high_score(uint32_t value) {
	high_score = value;
}

uint32_t get_high_score(void) {
	return high_score;
}

void manage_eeprom(void){
	uint32_t signature = eeprom_read_dword((uint32_t*)(4*0));
	if (signature == 0xBEEFBEEF) { //signature
		//get last 5 scores
		for (uint8_t i = 0; i < 5; i++) {
			loaded_scores[i] = eeprom_read_dword((uint32_t*)(4*(i+1)));
		}
	} else {
		//initialise values
		eeprom_write_dword(0, 0xBEEFBEEF);
		for (uint8_t i = 0; i < 5; i++) {
			eeprom_write_dword((uint32_t*)(4*(i+1)), 0);
			loaded_scores[i] = 0;
		}
		for (uint8_t i = 0; i < 5; i++) {
			for(uint8_t j = 0; j < 3; j++) {
				eeprom_write_byte((uint8_t*)(28+j+3*i), ' ');
			}
		}
	}
}

uint32_t * get_eeprom_scores(void) {
	for (uint8_t i = 0; i < 5; i++) {
		loaded_scores[i] = eeprom_read_dword((uint32_t*)(4*(i+1)));
	}
	return loaded_scores;
}

char * get_eeprom_initial(uint8_t index) {
	static char output[3];
	for (uint8_t i = 0; i < 3; i++) {
		output[i] = (char)eeprom_read_byte((uint8_t*)(28+i+(3*index)));	
	}
	return output;
}

void store_eeprom_score(uint32_t value, uint8_t index) {
	eeprom_write_dword((uint32_t*)(4*(index+1)),value);
}

void store_eeprom_initials(char *initials, uint8_t index) {
	for (uint8_t i = 0; i < 3; i++) {
		eeprom_write_byte((uint8_t*)(28+i+(3*index)), initials[i]);
	}
}

void wipe_eeprom(void) {
	eeprom_write_dword(0, 0xBEEFBEEF);
	for (uint8_t i = 0; i < 5; i++) {
		eeprom_write_dword((uint32_t*)(4*(i+1)), 0);
		loaded_scores[i] = 0;
	}
	for (uint8_t i = 0; i < 5; i++) {
		for(uint8_t j = 0; j < 3; j++) {
			eeprom_write_byte((uint8_t*)(28+j+3*i), ' ');
		}
	}
}
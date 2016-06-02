/*
 * score.c
 *
 * Written by Peter Sutton
 */

#include "score.h"
#include "game.h"

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
		wipe_eeprom();
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

uint8_t get_eeprom_save_state(void) {
	return eeprom_read_byte((uint8_t*)39);
}

FallingBlock get_eeprom_current_block(void) {
	FallingBlock output;
	output.blocknum = eeprom_read_byte((uint8_t*)184);
	output.row = eeprom_read_byte((uint8_t*)187);
	output.column = eeprom_read_byte((uint8_t*)188);
	output.rotation = eeprom_read_byte((uint8_t*)189);
	output.width = eeprom_read_byte((uint8_t*)190);
	output.height = eeprom_read_byte((uint8_t*)191);
	
	//get values for blockPattern and blockColour
	output.pattern = block_library[output.blocknum].patterns[output.rotation];
	output.colour = block_library[output.blocknum].colour;
	
	return output;
}

FallingBlock get_eeprom_next_block(void) {
	FallingBlock output;
	output.blocknum = eeprom_read_byte((uint8_t*)192);
	output.row = eeprom_read_byte((uint8_t*)195);
	output.column = eeprom_read_byte((uint8_t*)196);
	output.rotation = eeprom_read_byte((uint8_t*)197);
	output.width = eeprom_read_byte((uint8_t*)198);
	output.height = eeprom_read_byte((uint8_t*)199);
	
	//get values for blockPattern and blockColour
	output.pattern = block_library[output.blocknum].patterns[output.rotation];
	output.colour = block_library[output.blocknum].colour;
	
	return output;
}

uint8_t get_eeprom_rows_cleared(void) {
	return eeprom_read_byte((uint8_t*)200);
}

rowtype get_eeprom_board(uint8_t index) {
	return eeprom_read_byte((uint8_t*)(40+index));
}

MatrixColumn * get_eeprom_board_display(void) {
	static MatrixColumn output[16];
	for (uint8_t i = 0; i < 16; i++) {
		for (uint8_t j = 0; j < 8; j++) {
			output[i][j] = eeprom_read_byte((uint8_t*)(56+j+8*i));
		}
	}
	return output;
}

void write_eeprom_save_state(void) {
	eeprom_write_byte((uint8_t*)39, 1);
}

void write_eeprom_current_block(FallingBlock input) {
	eeprom_write_byte((uint8_t*)184,(uint8_t)input.blocknum);
	eeprom_write_byte((uint8_t*)187,(uint8_t)input.row);
	eeprom_write_byte((uint8_t*)188,(uint8_t)input.column);
	eeprom_write_byte((uint8_t*)189,(uint8_t)input.rotation);
	eeprom_write_byte((uint8_t*)190,(uint8_t)input.width);
	eeprom_write_byte((uint8_t*)191,(uint8_t)input.height);
}

void write_eeprom_next_block(FallingBlock input) {
	eeprom_write_byte((uint8_t*)192,(uint8_t)input.blocknum);
	eeprom_write_byte((uint8_t*)195,(uint8_t)input.row);
	eeprom_write_byte((uint8_t*)196,(uint8_t)input.column);
	eeprom_write_byte((uint8_t*)197,(uint8_t)input.rotation);
	eeprom_write_byte((uint8_t*)198,(uint8_t)input.width);
	eeprom_write_byte((uint8_t*)199,(uint8_t)input.height);
}

void write_eeprom_rows_cleared(uint8_t numRows) {
	eeprom_write_byte((uint8_t*)200, numRows);
}

void write_eeprom_board(rowtype input, uint8_t index) {
	eeprom_write_byte((uint8_t*)(40+index), input);
}

void write_eeprom_board_display(MatrixColumn * input) {
	for (uint8_t i = 0; i < 16; i++) {
		for (uint8_t j = 0; j < 8; j++) {
			eeprom_write_byte((uint8_t*)(56+j+8*i), (uint8_t)input[i][j]);
		}
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
	for (uint8_t i = 39; i < 201; i++) {
		eeprom_write_byte((uint8_t*)(1*i), 0);
	}
}
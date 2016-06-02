/*
 * score.h
 * 
 * Author: Peter Sutton
 */

#ifndef SCORE_H_
#define SCORE_H_

#include <stdint.h>
#include "pixel_colour.h"
#include "ledmatrix.h"
#include "blocks.h"

void init_score(void);
void add_to_score(uint16_t value);
uint32_t get_score(void);
uint32_t get_high_score(void);
void set_high_score(uint32_t value);
void manage_eeprom(void);
uint32_t* get_eeprom_scores(void);
void store_eeprom_score(uint32_t value, uint8_t index);
void store_eeprom_initials(char *initials, uint8_t index);
char * get_eeprom_initial(uint8_t index);
void wipe_eeprom(void);

uint8_t get_eeprom_save_state(void);

FallingBlock get_eeprom_current_block(void);

FallingBlock get_eeprom_next_block(void);

uint8_t get_eeprom_rows_cleared(void);

rowtype get_eeprom_board(uint8_t index);

MatrixColumn * get_eeprom_board_display(void);

void write_eeprom_save_state(void);

void write_eeprom_current_block(FallingBlock input);

void write_eeprom_next_block(FallingBlock input);

void write_eeprom_rows_cleared(uint8_t numRows);

void write_eeprom_board(rowtype input, uint8_t index);

void write_eeprom_board_display(MatrixColumn * input);

#endif /* SCORE_H_ */
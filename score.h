/*
 * score.h
 * 
 * Author: Peter Sutton
 */

#ifndef SCORE_H_
#define SCORE_H_

#include <stdint.h>

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

#endif /* SCORE_H_ */
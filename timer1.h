/*
 * timer0.h
 *
 * Author: Benedict Gattas
 *
 * 
 */

#ifndef TIMER1_H_
#define TIMER1_H_

#include <stdint.h>


void init_timer1(void);

void play_completed_row_tone(void);

void set_music_speed(uint32_t new_speed);

uint32_t get_music_speed(void);

void play_game_tone(uint8_t tone_number);

void switch_to_game_over(void);

#endif
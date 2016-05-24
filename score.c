/*
 * score.c
 *
 * Written by Peter Sutton
 */

#include "score.h"

// Variable to keep track of the score. We declare it as static
// to ensure that it is only visible within this module - other
// modules should call the functions below to modify/access the
// variable.
static uint32_t score;
static uint32_t high_score;

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

/*
 * terminalio.c
 *
 * Author: Peter Sutton
 */

#include <stdio.h>
#include <stdint.h>

#include <avr/pgmspace.h>

#include "terminalio.h"


void move_cursor(int8_t x, int8_t y) {
    printf_P(PSTR("\x1b[%d;%dH"), y, x);
}

void normal_display_mode(void) {
	printf_P(PSTR("\x1b[0m"));
}

void reverse_video(void) {
	printf_P(PSTR("\x1b[7m"));
}

void clear_terminal(void) {
	printf_P(PSTR("\x1b[2J"));
}

void clear_to_end_of_line(void) {
	printf_P(PSTR("\x1b[K"));
}

void set_display_attribute(DisplayParameter parameter) {
	printf_P(PSTR("\x1b[%dm"), parameter);
}

void hide_cursor() {
	printf_P(PSTR("\x1b[?25l"));
}

void show_cursor() {
	printf_P(PSTR("\x1b[?25h"));
}

void enable_scrolling_for_whole_display(void) {
	printf_P(PSTR("\x1b[r"));
}

void set_scroll_region(int8_t y1, int8_t y2) {
	printf_P(PSTR("\x1b[%d;%dr"), y1, y2);
}

void scroll_down(void) {
	printf_P(PSTR("\x1bM"));	// ESC-M
}

void scroll_up(void) {
	printf_P(PSTR("\x1b\x44"));	// ESC-D
}

void draw_horizontal_line(int8_t y, int8_t start_x, int8_t end_x) {
	int8_t i;
	move_cursor(start_x, y);
	reverse_video();
	for(i=start_x; i <= end_x; i++) {
		printf(" ");	/* No need to use printf_P - printing 
						 * a single character gets optimised
						 * to a putchar call 
						 */
	}
	normal_display_mode();
}

void draw_vertical_line(int8_t x, int8_t start_y, int8_t end_y) {
	int8_t i;
	move_cursor(x, start_y);
	reverse_video();
	for(i=start_y; i < end_y; i++) {
		printf(" ");
		/* Move down one and back to the left one */
		printf_P(PSTR("\x1b[B\x1b[D"));
	}
	printf(" ");
	normal_display_mode();
}

void display_score(uint32_t score){
	move_cursor(3,3);
	//print score
	//max value of uint16_t is 10 chars
	printf_P(PSTR("Score: %10d"), score);
}

void terminal_update_square(PixelColour value, int y_coord, int x_coord) {
	DisplayParameter color_code;
	//convert colours
	switch (value) {
		case COLOUR_BLACK :
			color_code = FG_BLACK;
			break;
		case COLOUR_RED :
			color_code = FG_RED;
			break;
		case COLOUR_GREEN :
			color_code = FG_GREEN;
			break;
		case COLOUR_YELLOW :
			color_code = FG_YELLOW;
			break;
		case COLOUR_ORANGE :
			color_code = FG_WHITE;
			break;
		case COLOUR_LIGHT_ORANGE :
			color_code = FG_CYAN;
			break;
		case COLOUR_LIGHT_YELLOW :
			color_code = FG_MAGENTA;
			break;
		case COLOUR_LIGHT_GREEN :
			color_code = FG_BLUE;
			break;
		default:
			color_code = FG_BLACK;
	}
	//output
	move_cursor(4 + x_coord, 6 + y_coord);
	set_display_attribute(color_code);
	printf("#");
	set_display_attribute(FG_WHITE);
}

void draw_game_area(void) {
	set_display_attribute(FG_WHITE);
	move_cursor(3, 5);
	printf_P(PSTR("##########"));
	for (int i = 0; i< 16; i++) {
		move_cursor(3, 6 + i);
		printf_P(PSTR("#        #"));
	}
	move_cursor(3, 22);
	printf_P(PSTR("##########"));
	
}
		
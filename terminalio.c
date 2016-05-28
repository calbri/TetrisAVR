/*
 * terminalio.c
 *
 * Author: Peter Sutton
 */

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>

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
	set_display_attribute(FG_WHITE);
	move_cursor(3,3);
	//print score
	//max value of uint16_t is 10 chars
	printf_P(PSTR("Score: %10d"), score);
}



void terminal_draw(MatrixData displayMatrix, int start, int numRows) {
	//memory allocation SHOULD BE MADE LOWER LATER ON
	//THIS IS VERY VERY IMPORTANT TO FUTURE CALLUM
	//WHO WILL READ THIS THE NIGHT BEFORE IT'S DUE
	char output[700];
	//initialise string
	strcpy(output,"");
	//move to top left corner
	move_cursor(4, 6);
	//color info strings
	char *color_code;
	char *prev_code;
	prev_code = "";
	//loop through current display matrix
	for (int i = start; i < numRows; i++) {
		for (int j = 0; j < BOARD_WIDTH; j++) {
			//convert colours
			switch (displayMatrix[i][j]) {
				case COLOUR_BLACK :
					color_code = "30";
					break;
				case COLOUR_RED :
					color_code = "31";
					break;
				case COLOUR_GREEN :
					color_code = "32";
					break;
				case COLOUR_YELLOW :
					color_code = "33";
					break;
				case COLOUR_ORANGE :
					color_code = "34";
					break;
				case COLOUR_LIGHT_ORANGE :
					color_code = "35";
					break;
				case COLOUR_LIGHT_YELLOW :
					color_code = "36";
					break;
				case COLOUR_LIGHT_GREEN :
					color_code = "37";
					break;
				default:
					color_code = "30";
			}
			if (strcmp(color_code,"30") == 0) {
				//black tile, make more efficient
				//by just sending a space
				//rather than a hash and colour info
				strcat(output, " ");
			} else {
				if (prev_code != color_code) {
					//add hash and colour code to string
					//if different
					strcat(output,"\x1b[");
					strcat(output,color_code);
					strcat(output,"m#");
					prev_code = color_code;
				} else {
					//colour is the same as the block before
					//no need to send colour info
					strcat(output, "#");
				}
			}
		}
		//add new line string and move cursor
		strcat(output,"\n");
		strcat(output,"\033[3C");
	}
	//output
	printf("%s",output);
}

void draw_game_window(void) {
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

void draw_next_block(FallingBlock block) {
	move_cursor(20, 10);
	printf("     ");
	move_cursor(20, 11);
	printf("     ");
	move_cursor(20, 12);
	printf("     ");
	move_cursor(20, 13);
	printf("     ");
	move_cursor(20, 14);
	printf("     ");
	move_cursor(20, 10);
	char output[50];
	char *color_code;
	strcpy(output, "");
	//convert colours
	switch (block.colour) {
		case COLOUR_BLACK :
		color_code = "30";
		break;
		case COLOUR_RED :
		color_code = "31";
		break;
		case COLOUR_GREEN :
		color_code = "32";
		break;
		case COLOUR_YELLOW :
		color_code = "33";
		break;
		case COLOUR_ORANGE :
		color_code = "34";
		break;
		case COLOUR_LIGHT_ORANGE :
		color_code = "35";
		break;
		case COLOUR_LIGHT_YELLOW :
		color_code = "36";
		break;
		case COLOUR_LIGHT_GREEN :
		color_code = "37";
		break;
		default:
		color_code = "30";
	}
	strcat(output,"\x1b[");
	strcat(output,color_code);
	strcat(output,"m");
	for(uint8_t row = 0; row < block.height; row++) {
		for(int col = (block.width - 1); col >= 0; col--) {
			if(block.pattern[row] & (1 << col)) {
				strcat(output, "#");
			} else {
				strcat(output, " ");
			}
		}
		//add new line string and move cursor
		strcat(output,"\n");
		strcat(output,"\033[19C");
	}
	printf("%s", output); 
}
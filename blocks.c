/*
 * blocks.c
 *
 * Written by Peter Sutton.
 */

#include "blocks.h"
#include "game.h"
#include "pixel_colour.h"
#include <stdlib.h>
/* Stdlib needed for random() - random number generator */

/*
 * Define the block library. 
 * Five blocks are defined initially.
 */

#define NUM_BLOCKS_IN_LIBRARY 5

// Block 0 (1 x 1) only has one pattern (rotation doesn't change this)
// -------*
#define BLOCK_0_HEIGHT 1
#define BLOCK_0_WIDTH 1
static rowtype block_0[] = { 0b1 };

// Block 1 (3 x 1) has two patterns
// -------* -----***
// -------*
// -------*
#define BLOCK_1_HEIGHT 3
#define BLOCK_1_WIDTH 1
static rowtype block_1_vert[] = { 0b1, 0b1, 0b1 };
static rowtype block_1_horiz[] = { 0b111 };
	
// Block 2 (2 x 2) has only one pattern
// ------**
// ------**
#define BLOCK_2_HEIGHT 2
#define BLOCK_2_WIDTH 2
static rowtype block_2[] = { 0b11, 0b11 };
	
// Block 3 (2 x 3) has four patterns
// ------*- ------*- -----*** -------*
// -----***	------** ------*- ------**
//          ------*-          -------*         
#define BLOCK_3_HEIGHT 2
#define BLOCK_3_WIDTH 3
static rowtype block_3_rot_0[] = { 0b010, 0b111 };
static rowtype block_3_rot_1[] = { 0b10, 0b11, 0b10 };
static rowtype block_3_rot_2[] = { 0b111, 0b010 };
static rowtype block_3_rot_3[] = { 0b01, 0b11, 0b01 };

// Block 4 (2 x 3) has four patterns
// -------* ------*- -----*** ------**
// -----*** ------*- -----*-- -------*
//          ------**          -------*
#define BLOCK_4_HEIGHT 2
#define BLOCK_4_WIDTH 3
static rowtype block_4_rot_0[] = { 0b001, 0b111 };
static rowtype block_4_rot_1[] = { 0b10, 0b10, 0b11 };
static rowtype block_4_rot_2[] = { 0b111, 0b100 };
static rowtype block_4_rot_3[] = { 0b11, 0b01, 0b01 };
	
static const BlockInfo block_library[NUM_BLOCKS_IN_LIBRARY] = {
	{ // Block 0
		COLOUR_RED, BLOCK_0_HEIGHT, BLOCK_0_WIDTH, 
		{ block_0, block_0, block_0, block_0 }
	},
	{ // Block 1
		COLOUR_ORANGE, BLOCK_1_HEIGHT, BLOCK_1_WIDTH,
		{ block_1_vert, block_1_horiz, block_1_vert, block_1_horiz }
	},
	{ // Block 2
		COLOUR_GREEN, BLOCK_2_HEIGHT, BLOCK_2_WIDTH,
		{ block_2, block_2, block_2, block_2 }
	},
	{ // Block 3
		COLOUR_YELLOW, BLOCK_3_HEIGHT, BLOCK_3_WIDTH,
		{ block_3_rot_0, block_3_rot_1, block_3_rot_2, block_3_rot_3 }		
	},
	{ // Block 4
		COLOUR_LIGHT_ORANGE, BLOCK_4_HEIGHT, BLOCK_4_WIDTH,
		{ block_4_rot_0, block_4_rot_1, block_4_rot_2, block_4_rot_3 }	
	}
};
	
	
FallingBlock generate_random_block(void) {
	FallingBlock block;	// This will be our return value

	// Pick a random block
	block.blocknum = random() % NUM_BLOCKS_IN_LIBRARY;
	
	// Initial rotation (no rotation by default)
	block.rotation = 0;	
	
	// Copy the relevant details of the block to our return value
	block.pattern = block_library[block.blocknum].patterns[block.rotation];
	block.colour = block_library[block.blocknum].colour;
	
	// Initial position (top right)
	block.row = 0;		// top row
	block.column = 0;	// rightmost column
	
	// Record the height and width of the block. We're using the default
	// rotation so this is just the height and width as in the block library
	block.height = block_library[block.blocknum].height;
	block.width = block_library[block.blocknum].width;
	
	return block;
}

/*
 * Attempt to rotate the given block clockwise by 90 degrees.
 * Returns 1 if successful (and modifies the given block) otherwise
 * returns 0 (and leaves the given block unchanged).
 * This method is only unsuccessful if the block is too close to the
 * left hand side to be rotated.
 */
int8_t rotate_block(FallingBlock* blockPtr) {
 	/* New block width will be the old height. New block height 
	 * will be the old width
	 */
	uint8_t new_width = blockPtr->height;
	uint8_t new_height = blockPtr->width;
	
	if(blockPtr->column + new_width > BOARD_WIDTH) {
		return 0;	// Block won't fit on the board if rotated
	}
	if(blockPtr->row + new_height > BOARD_ROWS) {
		return 0;	// Block will rotate off the bottom of the board
	}
	
	// Perform the rotation. We increment the rotation value (0 to 3)
	// and wrap back to 0 if we reach 4, i.e. add 1 and take mod 4.
	uint8_t new_rotation = (blockPtr->rotation + 1) % NUM_ROTATIONS;
	
	blockPtr->pattern = block_library[blockPtr->blocknum].patterns[new_rotation];
	blockPtr->rotation = new_rotation;
	blockPtr->width = new_width;
	blockPtr->height = new_height;
	
	// Rotation was successful
	return 1;
}

int8_t move_block_left(FallingBlock* blockPtr) {
	/* Check if the block is all the way to the left. If so, return 0
	 * because we can't shift it further to the left.
	 */
	if(blockPtr->column + blockPtr->width >= BOARD_WIDTH) {
		return 0;
	}

	/*
	 * Make the move.
	 */
	blockPtr->column += 1;
	return 1;
}

int8_t move_block_right(FallingBlock* blockPtr) {
	/* YOUR CODE HERE */
	
	/*
	 * You may wish to model it on move_block_left above
	 * Your function must return 0 if it's unable to move (e.g.
	 * block is against the right hand side), 1 otherwise.
	 */
	
	/*
	 * Initially, this function does nothing so we return 0
	 */
	return 0;
}

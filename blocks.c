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
	
	
FallingBlock generate_random_block(void) {
	FallingBlock block;	// This will be our return value

	// Pick a random block
	uint8_t randBlock = (random() % NUM_BLOCKS_IN_LIBRARY);
	block.blocknum = randBlock;
	
	// Initial rotation (no rotation by default)
	// ADDED: randomised out of the 4 options
	uint8_t randRotation = (random() % NUM_ROTATIONS); 
	block.rotation = randRotation;
	
	// Copy the relevant details of the block to our return value
	block.pattern = block_library[block.blocknum].patterns[block.rotation];
	block.colour = block_library[block.blocknum].colour;
	
	// Record the height and width of the block. Opposite rotations (i.e 0 and 2, 
	// 1 and 3) have the same width and height (due to symmetry)
	if (block.rotation % 2 == 0) {
		block.height = block_library[block.blocknum].height;
		block.width = block_library[block.blocknum].width;
	} else {
		block.height = block_library[block.blocknum].width;
		block.width = block_library[block.blocknum].height;
	}
	
	// Initial position (top right)
	block.row = 0;		// top row
	//determine randomized start point
	uint8_t randcol = random() % BOARD_WIDTH;
	//make sure it doesn't go off the left edge
	if (randcol+(block.width-1) >= BOARD_WIDTH) {
		block.column = BOARD_WIDTH-block.width;	// rightmost column that's allowed
	} else {
		block.column = randcol;
	}

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
	/* Check if the block is all the way to the right. If so, return 0
	 * because we can't shift it further to the right.
	 */
	if(blockPtr->column <= 0) {
		return 0;
	}
	
	/*
	 * You may wish to model it on move_block_left above
	 * Your function must return 0 if it's unable to move (e.g.
	 * block is against the right hand side), 1 otherwise.
	 */
	
	/*
	 * Make the move.
	 */
	blockPtr->column -= 1;
	return 1;
}

/*
 * blocks.h
 *
 * Written by Peter Sutton.
 */

#ifndef BLOCKS_H_
#define BLOCKS_H_

#include <stdint.h>
#include "pixel_colour.h"



/*
 * Type used to store row data. Must be able to hold BOARD_WIDTH number
 * of bits (defined in board.h)
*/
typedef uint8_t rowtype;

/*
 * Blocks are represented as bit patterns in an array of rows. We 
 * record as many rows as present in the block. Row 0 is the top of
 * the block. Column 0 (bit 0) in the row is at the right hand side.
 * Patterns are always aligned to the top right (i.e. row 0, bit 0).
 * For example, this block:
 *     -------*
 *     -------*
 *     ------**
 * would be represented as three rows with bit values 1, 1, 3
 *
 * The BlockPattern type is a pointer to the first member of this 
 * array of row data.
 */
typedef const rowtype* BlockPattern;

/*
 * Each block has 4 possible rotations. We record the bit pattern
 * associated with each rotation. Moving to a higher numbered
 * rotation in the array should be associated with a clockwise
 * rotation. We also record the colour of this block and the number
 * of rows and columns the block has (in the default (0) rotation).
 * These dimensions will also apply to rotation 2. Rotations 1 and 3
 * dimensions are given by swapping these row and column numbers.
 */
#define NUM_ROTATIONS 4
typedef struct {
	PixelColour colour;
	uint8_t height;	// Number of rows (in the default (0) rotation)
	uint8_t width;  // Number of columns (in the default (0) rotation)
	BlockPattern patterns[NUM_ROTATIONS];
} BlockInfo;

/*
 * Data for a falling block includes
 * - which block it is (0+ for block index)
 * - which pattern it has (will depend on the rotation)
 * - what colour it has
 * - current row on the board (rows are numbered from 0 at the top)
 * - current column on the board (columns are numbered from 0 at the right)
 * - current rotation (0 to 3 - indicating which block pattern is chosen)
 * - current width (may change if block is rotated)
 * - current height	(may change if block is rotated)
 */
typedef struct {
	int8_t blocknum;
	BlockPattern pattern;
	PixelColour colour;
	uint8_t row;
	uint8_t column;
	uint8_t rotation;
	uint8_t width;
	uint8_t height;
} FallingBlock;

/* 
 * Randomly choose a block from the block library and position
 * it at the top of the board.
 */
FallingBlock generate_random_block(void);

/*
 * Attempt to rotate the given block clockwise by 90 degrees.
 * Returns 1 if successful (and modifies the given block) otherwise
 * returns 0 (and leaves the given block unchanged). Rotation always 
 * happens about the top right position. Failure will occur if the 
 * block is currently taller than it is wide and too close to the left
 * hand side to allow rotation about the top right corner. (Such a block
 * should be moved to the right to allow rotation.)
 */
int8_t rotate_block(FallingBlock* blockPtr);

/* 
 * Attempt to move block one position to the left/right
 * Returns 1 if successful (and modifies the given block) otherwise
 * returns 0 (and leaves the block unchanged).
 * Failure happens only when the block is against the edge
 */
int8_t move_block_left(FallingBlock* blockPtr);
int8_t move_block_right(FallingBlock* blockPtr);

/*
 * Define the block library. 
 * Five blocks are defined initially.
 * TWO HAVE BEEN ADDED
 */

#define NUM_BLOCKS_IN_LIBRARY 7

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
	
// Block 5 (4 x 1) has two patterns
// -------* ----****
// -------*
// -------*
// -------*
#define BLOCK_5_HEIGHT 4
#define BLOCK_5_WIDTH 1
static rowtype block_5_horiz[] = { 0b1111 };
static rowtype block_5_vert[] = { 0b1, 0b1, 0b1, 0b1 };
	
// Block 6 (3 x 2) has four patterns
// -----*** -------* -----*-- ------**
// -------* -------* -----*** ------*-
//          ------**          ------*-
#define BLOCK_6_HEIGHT 2
#define BLOCK_6_WIDTH 3
static rowtype block_6_rot_0[] = { 0b111, 0b001 };
static rowtype block_6_rot_1[] = { 0b01, 0b01, 0b11 };
static rowtype block_6_rot_2[] = { 0b100, 0b111 };
static rowtype block_6_rot_3[] = { 0b11, 0b10, 0b10 };	
	
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
	},
	//EXTRA ONES ADDED
	{ // Block 5
		COLOUR_LIGHT_GREEN, BLOCK_5_HEIGHT, BLOCK_5_WIDTH,
		{ block_5_vert, block_5_horiz, block_5_vert, block_5_horiz }
	},
	{ // Block 6
		COLOUR_LIGHT_YELLOW, BLOCK_6_HEIGHT, BLOCK_6_WIDTH,
		{ block_6_rot_0, block_6_rot_1, block_6_rot_2, block_6_rot_3 }
	}
};

#endif /* BLOCKS_H_ */
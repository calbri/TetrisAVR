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

#endif /* BLOCKS_H_ */
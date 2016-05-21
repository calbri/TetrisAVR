/*
 * game.c
 *
 * Written by Peter Sutton.
 *
 * Game board data is stored in an array of rowtype (which is wide enough
 * to hold a bit for each column). The bits of the rowtype
 * represent whether that square is occupied or not (a 1 indicates
 * occupied). The least significant BOARD_WIDTH bits are used. The
 * least significant bit is on the right.
 */

#include "game.h"
#include "blocks.h"
#include "score.h"
#include "ledmatrix.h"

/*
 * Function prototypes.
 * game.h has the prototypes for functions in this module which
 * are available externally, and because we include "game.h" above
 * we do not need to repeat those prototypes.
 * 
 * The prototypes below are for the internal functions - not
 * accessible outside this module - so declared static.
 * The implementations of these functions are at the bottom
 * of the file - after the implementations of the publicly
 * available functions.
 */
static void check_for_completed_rows(void);
static uint8_t add_random_block(void);
static uint8_t block_collides(FallingBlock block);
static void remove_current_block_from_board_display(void);
static void add_current_block_to_board_display(void);

/*
 * Global variables.
 * We keep two representations of the board:
 *	- an array of "rowtype" rows (which has one bit per column
 *    which indicates whether the given position is occupied or not). This 
 *    representation does NOT include the current dropping block.
 *  - an array of corresponding LED matrix columns (a row of the game
 *    will be displayed on a column). This records colour information
 *    for each position. This DOES include the current dropping block.
 * For both representations, the array is indexed from row 0.
 * For "board" - column 0 (bit 0) is on the right
 * For "board_display" - element 0 within each MatrixColumn is on the left
 */
rowtype    board[BOARD_ROWS];
MatrixColumn board_display[BOARD_ROWS];
FallingBlock current_block;	// Current dropping block - there will 
							// always be one if the game is being played

/* 
 * Initialise board - all the row data will be empty (0) and we
 * create an initial random block and add it to the top of the board.
 */
void init_game(void) {	
	// Clear the LED matrix
	ledmatrix_clear();

	for(uint8_t row=0; row < BOARD_ROWS; row++) {
		board[row] = 0;
		for(uint8_t col=0; col < MATRIX_NUM_ROWS; col++) {
			board_display[row][col] = 0;
		}
	}
	// Adding a random block will update the "current_block" and 
	// add it to the board.	With an empty board this will always
	// succeed so we ignore the return value - this is indicated 
	// by the (void) cast. This function will update the display
	// for the required rows.
	(void)add_random_block();
}

/* 
 * Copy board to LED display for the rows given.
 * Note that each "row" in the board corresponds to a column for
 * the LED matrix.
 */
void update_rows_on_display(uint8_t row_start, uint8_t num_rows) {
	uint8_t row_end = row_start + num_rows - 1;
	for(uint8_t row_num = row_start; row_num <= row_end; row_num++) {
		ledmatrix_update_column(row_num, board_display[row_num]);
	}
}

/*
 * Attempt to move the current block to the left or right. 
 * This succeeds if
 * (1) the block isn't all the way to the side, and
 * (2) the board contains no blocks in that position.
 * Returns 1 if move successful, 0 otherwise.
 */
uint8_t attempt_move(int8_t direction) {	
	// Make a copy of the current block - we carry out the 
	// operations on the copy and copy it over to the current_block
	// if all is successful
	FallingBlock tmp_block = current_block;
	
	if(direction == MOVE_LEFT) {
		if(!move_block_left(&tmp_block)) {
			// Block was too far left - can't be moved
			return 0;
		}
	} else {
		// Attempt a move to the right
		if(!move_block_right(&tmp_block)) {
			// Block was too far right - can't be moved
			return 0;
		}
	}
	
	// The temporary block wasn't at the edge and has been moved
	// Now check whether it collides with any blocks on the board.
	if(block_collides(tmp_block)) {
		// Block will collide with other blocks so the move can't be
		// made.
		return 0;
	}
	
	// Block won't collide with other blocks so we can lock in the move.
	// First remove the current block from the display, update the current
	// block, then add it back to the board display
	remove_current_block_from_board_display();
	current_block = tmp_block;
	add_current_block_to_board_display();
	
	// Update the rows which are affected
	update_rows_on_display(current_block.row, current_block.height);
	return 1;
}

/*
 * Attempt to drop the current block by one row. This succeeds unless there
 * are squares blocked on the row below or we're at the bottom of
 * the board. Returns 1 if drop succeeded,  0 otherwise. 
 * (If the drop fails, the caller should add the block to the board.)
*/
uint8_t attempt_drop_block_one_row(void) {
	/*
	 * Check if the block has reached the bottom of the board.
	 * If so, do nothing and return false
	 */
	if(current_block.row + current_block.height >= BOARD_ROWS) {
		return 0;
	}
	
	/* Create a temporary block as a copy of the current block.
	 * Move it down 1 row and check whether it collides with
	 * any fixed blocks.
	 */
	FallingBlock tmp_block = current_block;
	tmp_block.row += 1;
	if(block_collides(tmp_block)) {
		// Block will collide if moved down - so we can't move it
		return 0;
	}
	
	// Move would succeed - so we make it happen
	remove_current_block_from_board_display();
	current_block = tmp_block;
	add_current_block_to_board_display();
	
	// Update the rows which are affected - starting from the row before
	// where the current block is.
	update_rows_on_display(current_block.row - 1, current_block.height + 1);
	
	// Move was successful - indicate so
	return 1;
}

/*
 * Attempt to rotate the block clockwise 90 degrees. Returns 1 if the
 * rotation is successful, 0 otherwise (e.g. a block on the board
 * blocks the rotation or the block is too close to the left edge to 
 * rotate).
 */
uint8_t attempt_rotation(void) {
	// Make a copy of the current block - we carry out the
	// operations on the copy and copy it back to the current_block
	// if all is successful
	FallingBlock tmp_block = current_block;
	
	if(!rotate_block(&tmp_block)) {
		// Block was too far left to rotate	- abort
		return 0;
	}
	
	// The temporary block has been rotated. 
	// Now check whether it collides with any blocks on the board.
	if(block_collides(tmp_block)) {
		// Block will collide with other blocks so the rotate can't be
		// made.
		return 0;
	}
	
	// Block won't collide with other blocks so we can lock in the move.
	// First determine the number of rows affected (to be redrawn) -
	// will be maximum of those in block before and after rotation
	uint8_t rows_affected = tmp_block.height;
	if(current_block.height > tmp_block.height) {
		rows_affected = current_block.height;
	}	
	
	// Second remove the current block from the display, update the current
	// block to the rotated version, then add it back to the board display
	remove_current_block_from_board_display();
	current_block = tmp_block;
	add_current_block_to_board_display();

	update_rows_on_display(current_block.row, rows_affected);
	
	// Rotation has happened - return true
	return 1;
}

/*
 * Add current block to board at its current position. We do this using a
 * bitwise OR for each row that contains the block.	No display update is
 * required. We then attempt to add a new block to the top of the board.
 * If this suceeds, we return 1, otherwise we return 0 (meaning game over).
 */
uint8_t fix_block_to_board_and_add_new_block(void) {
	for(uint8_t row = 0; row < current_block.height; row++) {
		uint8_t board_row = current_block.row + row;
		board[board_row] |= 
				(current_block.pattern[row]	<< current_block.column);
	}
	check_for_completed_rows();
	return add_random_block();
}

//////////////////////////////////////////////////////////////////////////
// Internal functions below
//////////////////////////////////////////////////////////////////////////
/* Function to check for completed rows on the board and remove them.
 * Higher rows are shifted down to occupy the removed rows. Empty (black)
 * rows are introduced at the top of the board. Both the board and 
 * board_display representations are updated. If any rows are complete and 
 * removed then we update the LED matrix. (Each row on the board corresponds
 * to a column on the LED matrix.)
 */
static void check_for_completed_rows(void) {
	/* YOUR CODE HERE */
	
	/* Suggested approach is to iterate over all the rows (0 to
	 * BOARD_ROWS -1) in the board and check if the row is all ones
	 * i.e. matches ((1 << BOARD_WIDTH) - 1).
	 * If a row of all ones is found, the rows above the current
	 * one should all be moved down one position and a zero (black)
	 * row inserted at the top. 
	 * Repeat this process if more than one completed row is
	 * found. If any completed rows ar found and removed then the 
	 * relevant rows of the display must also be updated.
	 *
	 * Note that both representations of the board must be updated:
	 * - board - which is the bitmap representation
	 * - board_display - which has the colours for each position in the row
	 * (You may find functions in ledmatrix.h useful for manipulating
	 * an LED matrix column structure - which corresponds to a row in the game.)
	 *
	 * EXAMPLE OF MOVES REQUIRED
	 * If rows 11 and 13 are completed (all ones in the
	 * board representation), then
	 * rows 14 and 15 at the bottom will remain unchanged
	 * old row 12 becomes row 13
	 * old row 10 becomes row 12
	 * old row 9 becomes row 11
	 * ...
	 * old row 0 becomes row 2
	 * row 1 (second top row) is set to 0 (black)
	 * row 0 (top row) is set to 0 (black)
	 */
	
}

/*
 * Add random block, return false (0) if we can't add the block - this
 * means the game is over, otherwise we return 1.
 */
static uint8_t add_random_block(void) {
	current_block = generate_random_block();
	// Check if the block will collide with the fixed blocks on the board
	if(block_collides(current_block)) {
		/* Block will collide. We don't add the block - just return 0 - 
		 * the game is over.
		 */
		return 0;
	}
	
	/* Block won't collide with fixed blocks on the board so 
	 * we update our board display.
	 */
	add_current_block_to_board_display();
	
	// Update the display for the rows which are affected
	update_rows_on_display(current_block.row, current_block.height);
	
	// The addition succeeded - return true
	return 1;
}

/*
 * Check whether the given block collides (intersects with) with
 * the fixed blocks on the board. Return 1 if it does collide, 0
 * otherwise.
 */
static uint8_t block_collides(FallingBlock block) {
	// We work out the bit patterns for the block in each row
	// and use a bitwise AND to determine whether there is an
	// intersection or not
	for(uint8_t row = 0; row < block.height; row++) {
		rowtype bit_pattern_for_row = block.pattern[row] << block.column;
		// The bit pattern to check this against will be that on the board
		// at the position where the block is located
		if(bit_pattern_for_row & board[block.row + row]) {
			// This row collides - we can stop now
			return 1;
		}
	}
	return 0;	// No collisions detected
}

/*
 * Remove the current block from the display structure
 */
static void remove_current_block_from_board_display(void) {
	for(uint8_t row = 0; row < current_block.height; row++) {
		uint8_t board_row = row + current_block.row;
		for(uint8_t col = 0; col < current_block.width; col++) {
			if(current_block.pattern[row] & (1 << col)) {
				// This position in the block is occupied - zero it out
				// in the display
				uint8_t board_column = col + current_block.column;
				uint8_t display_column = BOARD_WIDTH - board_column - 1;
				board_display[board_row][display_column] = 0;
			}
		}
	}
}

/*
 * Add the current block to the display structure
 */
static void add_current_block_to_board_display(void) {
	for(uint8_t row = 0; row < current_block.height; row++) {
		uint8_t board_row = row + current_block.row;
		for(uint8_t col = 0; col < current_block.width; col++) {
			if(current_block.pattern[row] & (1 << col)) {
				// This position in the block is occupied - add it to
				// the board display 
				uint8_t board_column = col + current_block.column;
				uint8_t display_column = BOARD_WIDTH - board_column - 1;
				board_display[board_row][display_column] = current_block.colour;
			}
		}
	}
}

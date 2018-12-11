#include "Util.h"

Position Util::SearchPatchPosByPatchID(int id)
{
	/* Suppose frame size is 16*8 and patch size is 4*4
	   There will be 4 columns and 2 rows that form 8 patches.
	   Suppose patch id is 6. What's the starting position(frame_row, frame_col) of that patch?
	   When count by hand (ignore the space between rows and columns), 
	   starting position is at (4,8).

	   In terms of formula, 
	   patch_row = 6 / 4 = 1
	   patch_col = 6 % 4 = 2

	   frame_row = 1 * 4 = 4
	   frame_col = 2 * 4 = 8

		0000 1111 2222 3333
		0000 1111 2222 3333
		0000 1111 2222 3333
		0000 1111 2222 3333
	
		4444 5555 6666 7777
		4444 5555 6666 7777
		4444 5555 6666 7777
		4444 5555 6666 7777

	*/

	int patch_row = id / PATCH_COLS_PER_FRAME;
	int patch_col = id % PATCH_COLS_PER_FRAME;

	int frame_row = patch_row * PATCH_WIDTH;
	int frame_col = patch_col * PATCH_HEIGHT;

	Position patchPos(frame_col, frame_row);
	return patchPos;
}
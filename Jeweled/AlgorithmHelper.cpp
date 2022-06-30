#include "AlgorithmHelper.h"
#include "AOJewel.h"
#include <assert.h>

////////////////////////////////////////////////////
// AlgorithmHelper
//  This class allows Jeweled to extract a lot of
//  its core 'match-3' functionality into generic
//  functions, as can be found below.
////////////////////////////////////////////////////

const unsigned short DEAD_JEWEL = 100;

void AlgorithmHelper::GenerateNewBoard(AOJewel* board[8][8], bool generateDropInPositions)
{
	// This function generates a new 8x8 board of jewels placed at random.

	// For speed of processing (and for ease of calculation) the jewels 
	//  taken in are converted to unsigned short values which represent
	//  their jewel ID. This is then converted back afterwards.

	// 1. Convert to unsigned short
	unsigned short boardRef[8][8];

	for (int i = 0; i < BOARD_SIZE; ++i)
	{
		for (int j = 0; j < BOARD_SIZE; ++j)
		{
			boardRef[i][j] = (unsigned short)board[i][j]->GetJewelType();
		}
	}

	// 2. Process generation of a new board
	GenerateNewBoard(boardRef);

	// 3. Convert back to jewels
	for (int i = 0; i < BOARD_SIZE; ++i)
	{
		for (int j = 0; j < BOARD_SIZE; ++j)
		{
			board[i][j]->SetJewelType((JEWEL_TYPE)boardRef[i][j]);
		}
	}


	// If drop-in positions are requested, the jewels will have their y
	//  positions allocated randomly above the board. They will appear
	//  to drop into place when the game starts.
	if (generateDropInPositions)
	{
		// add varying drop-in heights
		// Firstly move all jewels to above board
		for (int i = 0; i < BOARD_SIZE; ++i)
		{
			for (int j = 0; j < BOARD_SIZE; ++j)
			{
				// move 8 jewel (spaces) up
				board[i][j]->SetPosRelative(0, (int)(JEWEL_SIZE * -8));
				// touch - to start dropping
				board[i][j]->SetIsDropping(true);
			}
		}
		// Now we add varying levels of incremential random height adjustments
		for (int i = 0; i < BOARD_SIZE; ++i)
		{
			int yOffset = board[i][7]->GetYPos();
			for (int j = BOARD_SIZE - 1; j >= 0; j--)
			{
				const int VARIATION_DIFF = 200;
				yOffset -= (int)((rand() * 1.0f / RAND_MAX) * VARIATION_DIFF);
				board[i][j]->SetPosRelative(0, yOffset);
			}
		}
	}
}

void AlgorithmHelper::GenerateNewBoard(unsigned short board[8][8])
{
	// This function is responsible for actually generating a new board.
	// After generating an 8x8 board of unsigned short jewel IDs, it
	//  processes all existing match-3s on the board as if they were 
	//  ingame, until there are no more matches. If after this the board 
	//  has a suitable number of moves, it returns. Otherwise, repeats
	//  the whole process until a board is found.

	printf("AlgorithmHelper::GenerateNewBoard\n\n");
	int availableMoves = 0;

	do
	{
		for (int i = 0; i < BOARD_SIZE; ++i)
		{
			for (int j = 0; j < BOARD_SIZE; ++j)
			{
				unsigned short nextType = (unsigned short)(((float)rand() / RAND_MAX) * NUM_JEWEL_TYPES);
				board[i][j] = nextType;
			}
		}

		// Process existing matches
		int numToProcess = ProcessMatch3(board);
		while (numToProcess > 0)
		{
			ProcessDrop(board);
			numToProcess = ProcessMatch3(board);
		}

		availableMoves = CountAvailableMoves(board);
	} while (availableMoves < MIN_ALLOWED_STARTING_MOVES);

	//// hack for debug purposes: creates a vertical and horizontal match
	////  at the top left!
	//board[0][0] = 0;
	//board[0][1] = 0;
	//board[0][2] = 2;
	//board[1][0] = 0;
	//board[1][1] = 2;
	//board[1][2] = 0;
	//board[2][0] = 3;
	//board[2][1] = 0;
	//board[2][2] = 5;
}

unsigned int AlgorithmHelper::CountAvailableMoves(AOJewel* board[8][8])
{
	// This will return the number of available moves that can legally 
	//  be made that will cause a match to occur. This function simply
	//  converts the jewels to managable unsigned short values and
	//  passes the return value from having called that function.

	// Convert to unsigned short
	unsigned short boardRef[8][8];

	for (int i = 0; i < BOARD_SIZE; ++i)
	{
		for (int j = 0; j < BOARD_SIZE; ++j)
		{
			boardRef[i][j] = (unsigned short)board[i][j]->GetJewelType();
		}
	}

	return CountAvailableMoves(boardRef);
}
unsigned int AlgorithmHelper::CountAvailableMoves(unsigned short board[8][8])
{
	// This function scans the input board for all possible remaining legal
	//  moves (matches that can be made). It returns the number of unique valid 
	//  moves that can be made.

	// All possible moves that can be found can be reduced to four basic shapes,
	//  which I label 'J', 'L', 'C' and 'i': ('#' representing a main colour,
	//  '_' = other)

	//  J    L    C    i 
	//                _#_
	// _#_  _#_  _#_  ___
	// _#_  _#_  #__  _#_
	// #__  __#  _#_  _#_

	// Simple rotations of these shapes can cover all possible available
	//  jewel matches. Given the useful 3x3 grid which the first three shapes
	//  fit into, I match each of these together. The 'i' shape must be matched
	//  seperately, as it covers a 4x3 area.

	// The algorithm scans for 3 match-types at 4 rotations, across an
	//  (8-2)*(8-2) board size and scans for 1 match-type at 4 rotations,
	//   across a (8-3)*(8) board size:
	// ( 3*4*(8-2)*(8-2) ) + ( 1*4*(8-3)*(8) ) == 432 + 160 == 592 comparisons..
	//  ...can this be optimized?

	printf("AlgorithmHelper::CountAvailableMoves\n");

	unsigned int retVal = 0; // available moves

	for (int i = 0; i < BOARD_SIZE - 2; ++i)
	{
		for (int j = 0; j < BOARD_SIZE - 2; ++j)
		{
			unsigned short innerBoard[3][3] =
			{ { board[i][j],   board[i + 1][j],   board[i + 2][j] },
			{ board[i][j + 1], board[i + 1][j + 1], board[i + 2][j + 1] },
			{ board[i][j + 2], board[i + 1][j + 2], board[i + 2][j + 2] } };

			// check for:
			//  J    L    C      
			// ___  __#  ___  rotation 1
			// ##_  ##_  #_#
			// __#  ___  _#_

			// __#  #__  _#_  rotation 2
			// _#_  _#_  __#
			// _#_  _#_  _#_

			// #__  ___  _#_  rotation 3
			// _##  _##  #_#
			// ___  #__  ___

			// _#_  _#_  _#_  rotation 4
			// _#_  _#_  #__
			// #__  __#  _#_

			for (int rot = 0; rot < 4; ++rot)
			{
				if (innerBoard[0][1] == innerBoard[1][1] &&
					innerBoard[1][1] == innerBoard[2][2])
				{
					// J-type
					retVal++;
					printf(" Match: rot %i, type J at %d, %d\n", rot + 1, i + 2, j + 2);
				}
				if (innerBoard[0][1] == innerBoard[1][1] &&
					innerBoard[1][1] == innerBoard[0][2])
				{
					// L-type
					retVal++;
					printf(" Match: rot %i, type L at %d, %d\n", rot + 1, i, j);
				}
				if (innerBoard[0][1] == innerBoard[1][2] &&
					innerBoard[1][2] == innerBoard[2][1])
				{
					// C-type
					retVal++;
					printf(" Match: rot %i, type C at %d, %d\n", rot + 1, i, j);
				}

				// rotate board and repeat!
				unsigned short oldBoard[3][3];
				memcpy_s(oldBoard, 9 * sizeof(unsigned short),
					innerBoard, 9 * sizeof(unsigned short));

				innerBoard[0][0] = oldBoard[2][0];
				innerBoard[0][1] = oldBoard[1][0];
				innerBoard[0][2] = oldBoard[0][0];
				innerBoard[1][2] = oldBoard[0][1];
				innerBoard[2][2] = oldBoard[0][2];
				innerBoard[2][1] = oldBoard[1][2];
				innerBoard[2][0] = oldBoard[2][2];
				innerBoard[1][0] = oldBoard[2][1];
			}

		}
	}

	// NB. Special match: i
	// ____  
	// ##_#  and all it's rotations/variations
	// ____ 

	// Find all ##_# matches using 2 pairs of passes
	// 1. Check rows
	for (int i = 0; i < BOARD_SIZE; ++i)
	{
		for (int j = 0; j < BOARD_SIZE - 3; ++j)
		{
			if (board[i][j] == board[i][j + 1] && board[i][j + 1] == board[i][j + 3])
			{
				retVal++;
				printf(" Match: type i (##_#) at %d, %d\n", i, j);
			}
			if (board[i][j] == board[i][j + 2] && board[i][j + 2] == board[i][j + 3])
			{
				retVal++;
				printf(" Match: type i (#_##) at %d, %d\n", i, j);
			}
		}
	}
	// 2. Check columns
	for (int i = 0; i < BOARD_SIZE - 3; ++i)
	{
		for (int j = 0; j < BOARD_SIZE; ++j)
		{
			if (board[i][j] == board[i + 2][j] && board[i + 2][j] == board[i + 3][j])
			{
				retVal++;
				printf(" Match: type i at %d, %d\n", j, j);
			}
			if (board[i][j] == board[i + 1][j] && board[i + 1][j] == board[i + 3][j])
			{
				retVal++;
				printf(" Match: type i (Inverted: !) at %d, %d\n", i, j);
			}
		}
	}

	printf("Retval: %d\n\n", retVal);
	return retVal;
}

unsigned int AlgorithmHelper::ProcessMatch3(AOJewel* board[8][8])
{
	// This function will be called when the board needs to be scanned for all
	//  matched jewels that need to be removed.

	// After converting the board to unsigned short values for processing and
	//  passing to ProcessMatch(unsigned short[8][8]), this function will then
	//  check for all values that have been set to DEAD_JEWEL (100), and will
	//  call Remove() on all corresponding jewel objects.

	// Convert the board to unsigned short[8][8] -> boardRef
	unsigned short boardRef[8][8];
	for (int i = 0; i < BOARD_SIZE; ++i)
	{
		for (int j = 0; j < BOARD_SIZE; ++j)
		{
			boardRef[i][j] = (unsigned short)board[i][j]->GetJewelType();
		}
	}

	unsigned int numMatches = ProcessMatch3(boardRef);

	// if we have nothing to do, don't bother with the extra for() loop.
	if (numMatches == 0)
		return 0;

	// For all values in boardRef that are 100, set the corresponding
	// jewel to Remove().
	for (int i = 0; i < BOARD_SIZE; ++i)
	{
		for (int j = 0; j < BOARD_SIZE; ++j)
		{
			if (boardRef[i][j] == DEAD_JEWEL)
			{
				// this jewel should be removed
				board[i][j]->Remove();
			}
		}
	}

	return numMatches;
}

unsigned int AlgorithmHelper::ProcessMatch3(unsigned short board[8][8])
{
	// This function finds all sets of three or more of the same array value
	//  and sets them to be '100', which we can later Remove() the 
	//  corresponding jewel.

	// To do this successfully, we first clone the board. From this clone we
	//  can scan for all chains of three. For each chain of three in the clone
	//  board, we set the corresponding value in the original board to '100'.
	// In doing this it becomes possible to check for chains of 4, 5, as we 
	//  never alter the cloned copy of the board.

	printf("AlgorithmHelper::ProcessMatch3\n");
	// find all *** horz and vert
	// store them as destroyed 100 in newBoard
	unsigned short newBoard[BOARD_SIZE][BOARD_SIZE];
	unsigned short retVal = 0;

	memcpy_s(newBoard, BOARD_SIZE * BOARD_SIZE * sizeof(unsigned short),
		board, BOARD_SIZE * BOARD_SIZE * sizeof(unsigned short));

	// find all horizontal triplets
	for (int i = 0; i < BOARD_SIZE; ++i)
	{
		for (int j = 0; j < BOARD_SIZE - 2; ++j)
		{
			if (board[i][j] == board[i][j + 1] &&
				board[i][j + 1] == board[i][j + 2])
			{
				newBoard[i][j] = DEAD_JEWEL;
				newBoard[i][j + 1] = DEAD_JEWEL;
				newBoard[i][j + 2] = DEAD_JEWEL;
				retVal++;
			}
		}
	}

	// find all vertical triplets
	for (int i = 0; i < BOARD_SIZE - 2; ++i)
	{
		for (int j = 0; j < BOARD_SIZE; ++j)
		{
			if (board[i][j] == board[i + 1][j] &&
				board[i + 1][j] == board[i + 2][j])
			{
				newBoard[i][j] = DEAD_JEWEL;
				newBoard[i + 1][j] = DEAD_JEWEL;
				newBoard[i + 2][j] = DEAD_JEWEL;
				retVal++;
			}
		}
	}

	memcpy_s(board, BOARD_SIZE * BOARD_SIZE * sizeof(unsigned short),
		newBoard, BOARD_SIZE * BOARD_SIZE * sizeof(unsigned short));

	printf("Retval: %d\n\n", retVal);
	return retVal;
}

bool AlgorithmHelper::ProcessDrop(unsigned short board[8][8])
{
	// This function performs a single pseudo drop of jewel IDs in a board.
	// It treats IDs of 100 as 'dead' jewels. It does not wait to call
	//  Remove() (like normal), as we are not dealing with jewels, but
	//  with jewel IDs. It instantly replaces all jewels as if they fell
	//  from the top of the board.

	printf("AlgorithmHelper::ProcessDrop\n\n");

	// 1. Reverse scan of board to 'pull' jewels down
	for (int i = BOARD_SIZE - 1; i >= 0; --i)
	{
		for (int j = BOARD_SIZE - 1; j >= 1; --j) // DO NOT DO for top row!
		{
			if (board[i][j] == DEAD_JEWEL)
			{
				board[i][j] = board[i][j - 1];
				board[i][j - 1] = DEAD_JEWEL;
			}
		}
	}

	// 2. Scan for '100's and replace with new jewels ( i.e. drops from top of screen )
	for (int i = 0; i < BOARD_SIZE; ++i)
	{
		for (int j = 0; j < BOARD_SIZE; ++j)
		{
			if (board[i][j] == DEAD_JEWEL)
			{
				unsigned short newJewelID = (unsigned short)(((float)rand() / RAND_MAX) * NUM_JEWEL_TYPES);
				board[i][j] = newJewelID;
			}
		}
	}

	return true;
}

void AlgorithmHelper::SwapJewels(AOJewel* pFirst, AOJewel* pSecond, AOJewel* board[8][8])
{
	//	This swaps two jewel pointers in a board, given the two jewel pointers.
	//	This is used by the GameBoard object when a user-swap has occurred.
	//	A single pass of ProcessMatch3 then takes place, and if no match can
	//   be found the pointers are swapped back again by this function.

	// 1. find first jewel's coordinates from within board
	// 2. find second jewel's coordinates from within board
	// 3. perform swap

	//	Sanity check
	assert(pFirst != pSecond);

	printf("AlgorithmHelper::SwapJewels()... ");

	unsigned short firstX, firstY, secondX, secondY;

	for (int i = 0; i < BOARD_SIZE; ++i)
	{
		for (int j = 0; j < BOARD_SIZE; ++j)
		{
			//	NOTE: pointer comparison - object identity!
			if (board[i][j] == pFirst)
			{
				firstX = i; firstY = j;
			}
			//	NOTE: pointer comparison - object identity!
			if (board[i][j] == pSecond)
			{
				secondX = i; secondY = j;
			}
		}
	}

	//	perform swap
	AOJewel* pTemp = board[firstX][firstY];
	board[firstX][firstY] = board[secondX][secondY];
	board[secondX][secondY] = pTemp;

	printf("swapped [%d][%d] and [%d][%d]\n", firstX, firstY, secondX, secondY);
}

void AlgorithmHelper::RemoveAndSpawn(AOJewel* jewel, AOJewel* board[8][8])
{
	// This should be called when a jewel has completed it's 'dying' animation
	//  and is ready to be respawned at the top of the board.
	// All members of the board directly above this jewel need to be informed
	//  that they should aim to drop a further 52 pixels.
	// This algorithm has the responsibility of doing all the above for this
	//  one jewel.

	printf("\nRemoveAndSpawn()\n");

	// 1. find this jewel's column
	unsigned short col, row;
	for (int i = 0; i < 8; ++i)
	{
		for (int j = 0; j < 8; ++j)
		{
			if (board[j][i] == jewel)
			{
				col = j;
				row = i;
			}
		}
	}

	printf(" Dealing with dead jewel at [%d][%d]\n", col, row);

	// From now we are only going to be using board[col][]

	// 2. Physically swap each of the jewels above this jewel.
	//    Swap all jewels bottom-up from the current jewel (at [col][row] )
	//     for all jewels that are still in the board (up to 0, non-inclusive)
	for (int i = row; i > 0; --i)
	{
		printf("Swap [%d][%d]-[%d][%d], \n", col, i, col, i - 1);
		// perform swap
		AOJewel* temp = board[col][i];
		board[col][i] = board[col][i - 1];
		board[col][i - 1] = temp;
	}

	// 3. reactivate dead jewel
	jewel->SetIsDead(false);

	// 4. set new position for jewel (52 above the highest jewel!)
	int targetYHeight = 999;
	//for ( int i = 0; i < 8; ++i )
	//{
	//	if ( board[col][i]->GetYPos() < targetYHeight )
	//		targetYHeight = board[col][i]->GetYPos();
	//}
	//printf("Highest jewel on this column is at y:%d\n", targetYHeight );
	//targetYHeight -= JEWEL_SIZE;

	// Hack to get this working! TODO: This sets the new location for spawn-in
	//  jewels to be a jewel above the board
	targetYHeight = MAIN_BOARD_OFFSET_Y - JEWEL_SIZE;

	printf(" Jewel move: (y:%d)->(y:%d) \n", jewel->GetYPos(), targetYHeight);
	jewel->SetPos(jewel->GetXPos(), targetYHeight);
	jewel->ResetDropTarget();

	// 5. set new drop positions for each of these jewels affected
	//  ( including top jewel at [0] )
	for (int i = 0; i <= row; ++i)
	{
		// calc new drop target
		printf("For jewel at [%d][%d], ", col, i);
		board[col][i]->SetDropTargetRelative(JEWEL_SIZE);
	}

#ifdef DEBUG
	// Debug: log of logical jewel locations
	int x, y;
	EstimateJewelBoardLocation(jewel, x, y);
	printf("xy pos suggests that the current jewel is at [%d][%d]\n", x, y);

	// Debug: drop target checks
	printf(" Jewel, \ty pos, \tdrop target\n");
	for (int i = 0; i < 8; ++i)
		printf(" [%d][%d]: %d\t%d\n", col, i, board[col][i]->GetYPos(),
			board[col][i]->GetDropTarget());

	// Post-implementation checks
	//for ( int i = 0; i < 8; ++i )
	//	assert(board[col][i]->GetDropTarget() >= 0 );

#endif //DEBUG

	printf("End RemoveAndSpawn()\n");
}

void AlgorithmHelper::EstimateJewelBoardLocation(AOJewel* jewel, int& x, int& y)
{
	// Given a jewel object this function should be able to provide a
	//  'most-likely' readible grid coordinate. 
	// IMPORTANT: This calculation is allowed to give a negative coordinate,
	//  so DO NOT use this to index into arrays! Hence the name 'estimate' -
	//  this gives 'literal' visual coordinates.

	int xLoc = jewel->GetXPos() - MAIN_BOARD_OFFSET_X;
	int yLoc = jewel->GetYPos() - MAIN_BOARD_OFFSET_Y;

	xLoc = (int)(xLoc / JEWEL_SIZE);
	yLoc = (int)(yLoc / JEWEL_SIZE);

	x = xLoc;
	y = yLoc;
}
#pragma once

class AOJewel;

// static class
class AlgorithmHelper
{
public:
	static void EstimateJewelBoardLocation(AOJewel* jewel, int& x, int& y);
	static void GenerateNewBoard(AOJewel* board[8][8], bool generateDropInPositions);
	static void GenerateNewBoard(unsigned short board[8][8]);

	static unsigned int CountAvailableMoves(AOJewel* board[8][8]);
	static unsigned int CountAvailableMoves(unsigned short board[8][8]);

	static unsigned int ProcessMatch3(AOJewel* board[8][8]);
	static unsigned int ProcessMatch3(unsigned short board[8][8]);
	static bool ProcessDrop(unsigned short board[8][8]);

	static void SwapJewels(AOJewel* pFirst, AOJewel* pSecond, AOJewel* board[8][8]);

	static void RemoveAndSpawn(AOJewel* pJewel, AOJewel* board[8][8], int numPriorCascadesThisColumn);
};

// Assert that there must be how many moves available at the start of the game:
static const unsigned int MIN_ALLOWED_STARTING_MOVES = 10;
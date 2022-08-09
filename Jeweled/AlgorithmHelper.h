#pragma once

#include <vector>

class AOJewel;

template<typename T>
T Max(T val, T max)
{
	return val > max ? max : val;
}

// static class
class AlgorithmHelper
{
public:
	struct MatchData
	{
		int xStart;
		int yStart;
		bool bIsHorizontalMatch;
		int numSymbols;
	};

public:
	static void EstimateJewelBoardLocation(AOJewel* jewel, int& x, int& y);
	static void GenerateNewBoard(AOJewel* board[8][8], bool generateDropInPositions);
	static void GenerateNewBoard(unsigned short board[8][8]);

	static unsigned int CountAvailableMoves(AOJewel* board[8][8]);
	static unsigned int CountAvailableMoves(unsigned short board[8][8]);

	//	Returns number of triplets found
	//	TBD: naming of these two functions needs improving
	static unsigned int ProcessMatch3(AOJewel* board[8][8], std::vector<MatchData>& vMatchFoundData);
	static unsigned int ProcessMatch3(unsigned short board[8][8], std::vector<MatchData>& vMatchFoundData);
	static bool ProcessDrop(unsigned short board[8][8]);

	static void SwapJewels(AOJewel* pFirst, AOJewel* pSecond, AOJewel* board[8][8]);

	static void RemoveAndSpawn(AOJewel* pJewel, AOJewel* board[8][8], int numPriorCascadesThisColumn);
};

// Assert that there must be how many moves available at the start of the game:
static const unsigned int MIN_ALLOWED_STARTING_MOVES = 10;
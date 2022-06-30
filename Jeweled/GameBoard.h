#pragma once

#include "SDL.h"
#include "ActiveObject.h"
#include "AlgorithmHelper.h"
#include <string>

class ActiveObject;
class AOJewel;
class AOSelectionCursor;
class AOButton;

const unsigned short BOARD_SIZE = 8;
const unsigned short JEWEL_SIZE = 52;
const unsigned short NUM_JEWEL_TYPES = 7;
const unsigned short MAIN_BOARD_OFFSET_X = 200;
const unsigned short MAIN_BOARD_OFFSET_Y = 16;

//	See gemsheet6.png for jewel images
enum JEWEL_TYPE 
{ 
	JEWEL_YELLOW = 0, 
	JEWEL_WHITE, 
	JEWEL_BLUE, 
	JEWEL_RED,
	JEWEL_PURPLE, 
	JEWEL_ORANGE, 
	JEWEL_GREEN, 
	JEWEL_UNKNOWN 
};

//	For use when swapping jewels
enum SWAP_TYPE 
{ 
	SWAP_TYPE_NONE = 0, 
	SWAP_TYPE_ABOVE, 
	SWAP_TYPE_BELOW, 
	SWAP_TYPE_LEFT, 
	SWAP_TYPE_RIGHT 
};

class GameBoard
{
public:
	explicit GameBoard(const std::string& windowSpriteID, const std::string& gemSpriteSheetID, const std::string& backgroundTileSheetID);

public:
	int Init();
	int Update();
	int Render();

	bool GenerateNextTileBackground( const unsigned int level );

	bool HandleMouseClickDownAt( const int& x, const int& y );
	bool HandleMouseClickUpAt( const int& x, const int& y );
	bool HandleMouseHoverAt( const int& x, const int& y );

	bool EngagePauseMode();
	bool ResumeFromPauseMode();
	bool IsPaused()							{ return m_bPauseEnabled; }
	Rect GetBoardRect();

#ifdef _DEBUG
	void DebugDump();
#endif //_DEBUG

	void ResetBoard( bool newGame )			{ AlgorithmHelper::GenerateNewBoard(m_board, newGame ); }
	unsigned int CountAvailableMoves()		{ return AlgorithmHelper::CountAvailableMoves( m_board ); }

private: //member functs
	AOJewel* GetJewelAt( const int& x, const int& y );

private: //variables
	const unsigned int m_jewelsWide = 8;
	const unsigned int m_jewelsTall = 8;
	std::string m_windowSpriteID;
	std::string m_gemSpriteSheetID;
	std::string m_backgroundTileSheetID;
	
	AOJewel* m_board[BOARD_SIZE][BOARD_SIZE]; // uses AOJewel* references

	AOJewel* m_selectedJewel; 
	AOJewel* m_secondJewel;

	AOButton* m_newGame;
	AOButton* m_newGameNormal;
	AOButton* m_newGameTimed;
	AOButton* m_options;
	AOButton* m_hint;
	AOButton* m_quit;

	SWAP_TYPE m_swapType; // for use when swapping is occuring

	AOSelectionCursor* m_selectionCursor; // sprite

	// For active background switching
	SDL_Texture* m_tileBackground;
	SDL_Texture* m_nextBackground;

	bool m_bJewelsInMotion; // true if some jewel somewhere is already falling
						   // (so no additional swapping is allowed)
	bool m_bPerformingSwap; // true if a swap animation is taking place
						   // (so no interaction whatsoever is allowed)
	bool m_bPauseEnabled;   // true if app is paused
	
	int m_numMovesAvailable;
	int m_cascadeAmount; // number of continuous ongoing cascades ( usually 0 )

public:
	bool m_bNoRandCol;
	bool m_bHideGems;
};

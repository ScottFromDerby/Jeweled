#include "GameBoard.h"
#include "AlgorithmHelper.h"
#include "MouseCursor.h"
#include "SDLAudio.h"
#include "SDLRenderer.h"
#include "SDL.h"

#include "ActiveObject.h"
#include "AOJewel.h"
#include "AOSelectionCursor.h"
#include "AOButton.h"

#include <assert.h>
#include <math.h>

////////////////////////////////////////////////////
// A GameBoard object.
//  Contains all jewels, and methods for interacting
//  with the gameboard. Also contains various other
//  ActiveObjects, such as buttons and dialogs.
////////////////////////////////////////////////////


GameBoard::GameBoard(const std::string& windowSpriteID, const std::string& gemSpriteSheetID, const std::string& backgroundTileSheetID)
	: m_windowSpriteID(windowSpriteID),
	m_gemSpriteSheetID(gemSpriteSheetID),
	m_backgroundTileSheetID(backgroundTileSheetID),
	m_bNoRandCol(false),
	m_bHideGems(false),
	m_bPauseEnabled(false),
	m_bPerformingSwap(false),
	m_swapType(SWAP_TYPE_NONE),
	m_cascadeAmount(0),
	m_hint(nullptr),
	m_bJewelsInMotion(false),
	m_newGame(nullptr),
	m_newGameNormal(nullptr),
	m_newGameTimed(nullptr),
	m_nextBackground(nullptr),
	m_numMovesAvailable(0),
	m_options(nullptr),
	m_quit(nullptr),
	m_secondJewel(nullptr),
	m_selectedJewel(nullptr),
	m_selectionCursor(nullptr),
	m_tileBackground(nullptr)
{
}

int GameBoard::Init()
{
	printf("GameBoard::Init()\n");
	//const int JEWEL_SPACING = 1;
	const int NUM_JEWELS = 7;

	for (unsigned int i = 0; i < m_jewelsTall; ++i)
	{
		for (unsigned int j = 0; j < m_jewelsWide; ++j)
		{
			VEC2 pos;
			pos.x = MAIN_BOARD_OFFSET_X + (JEWEL_SPACING) * j;
			pos.y = MAIN_BOARD_OFFSET_Y + (JEWEL_SPACING) * i;

			VEC2 size(JEWEL_SIZE, JEWEL_SIZE);

			AOJewel* nextJewel = new AOJewel(pos, size, 0, m_gemSpriteSheetID);

			nextJewel->Init();
			nextJewel->m_initialGridXPos = j;
			nextJewel->m_initialGridYPos = i;

			m_board[j][i] = nextJewel;
		}
	}

	AlgorithmHelper::GenerateNewBoard(m_board, false);

	m_tileBackground = SDL_CreateTexture(SDLRenderer::Get()->GetRenderer(), SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_TARGET, 512, 512);
	m_nextBackground = SDL_CreateTexture(SDLRenderer::Get()->GetRenderer(), SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_TARGET, 512, 512);

	GenerateNextTileBackground(1);
	//m_tileBackground = m_nextBackground;

	// Selection Cursor
	VEC2 selCursorSize(SELECTION_CURSOR_SIZE, SELECTION_CURSOR_SIZE);
	m_selectionCursor = new AOSelectionCursor(selCursorSize, "SelectionCursor");
	m_selectionCursor->Init();

	m_selectedJewel = nullptr;

	SDLRenderer::Get()->AddImage("..\\Resources\\over1-hint1.png", "Hint-Hover");
	SDLRenderer::Get()->AddImage("..\\Resources\\over2-newgame-pushed1.png", "NewGame-Pushed");
	SDLRenderer::Get()->AddImage("..\\Resources\\over3-normal-pushed1.png", "Normal-Pushed");
	SDLRenderer::Get()->AddImage("..\\Resources\\over4-timetrial-pushed1.png", "Timed-Pushed");
	SDLRenderer::Get()->AddImage("..\\Resources\\over5-options-pushed1.png", "Options-Pushed");
	SDLRenderer::Get()->AddImage("..\\Resources\\over6-quit-pushed1.png", "Quit-Pushed");
	SDLRenderer::Get()->AddImage("..\\Resources\\over7-normal-selected1.png", "Normal-Selected");
	SDLRenderer::Get()->AddImage("..\\Resources\\over8-timetrial-selected1.png", "Timed-Selected");

	m_newGame = new AOButton(VEC2(20, 144), VEC2(156, 41), "", "", "NewGame-Pushed", "", BUTTON_TYPE_NEWGAME);
	m_newGameNormal = new AOButton(VEC2(20, 188), VEC2(78, 31), "", "", "Normal-Pushed", "Normal-Selected", BUTTON_TYPE_NG_NORMAL);
	m_newGameTimed = new AOButton(VEC2(96, 188), VEC2(78, 31), "", "", "Timed-Pushed", "Timed-Selected", BUTTON_TYPE_NG_TIMED);
	m_options = new AOButton(VEC2(20, 244), VEC2(156, 41), "", "", "Options-Pushed", "", BUTTON_TYPE_OPTIONS);
	m_hint = new AOButton(VEC2(65, 305), VEC2(52, 72), "", "Hint-Hover", "", "", BUTTON_TYPE_HINT);
	m_quit = new AOButton(VEC2(20, 404), VEC2(156, 40), "", "", "Quit-Pushed", "", BUTTON_TYPE_QUIT);

	for (int i = 0; i < BOARD_SIZE; ++i)
	{
		m_numNewJewelsThisCascadePerColumn[i] = 0;
	}
	
	return 0;
}

int GameBoard::Update()
{
	// GameLogic here!
	m_bJewelsInMotion = false; // until proven true

	if (m_bPauseEnabled)
	{
		// fallthrough for pause-specific functions
		// however... when bejeweled is paused, *nothing* different happens...
	}
	else
	{
		if (m_bPerformingSwap)
		{
			m_bJewelsInMotion = true;
			// Swap is happening - so we perform the first half of the animation.

			// This is pseudo-pixel animation...
			// Must add up to 52, then reduce to 0 for pixel accuracy!
			const int STEP_PIXEL_COUNT[] =
			{ 1,1,1,1,2,2,2,2,2,3,3,3,3,3,3,3,3,2,2,2,2,2,1,1,1,1,
				-1,-1,-1,-1,-2,-2,-2,-2,-2,-3,-3,-3,-3,-3,-3,-3,-3,-2,-2,-2,-2,
				-2,-1,-1,-1,-1 };

			// actual start index
			const int SWAP_STEP_START = 0;
			// actual end index
			const int SWAP_STEP_END = sizeof(STEP_PIXEL_COUNT) / sizeof(int) - 1;
			// point at which a full switch has occured
			const int SWAP_STEP_MIDDLE = (int)(SWAP_STEP_END / 2) + 1;

			static int swapAnimationStep = 0;
			static int movementSpeed = 0;

			if (swapAnimationStep == SWAP_STEP_MIDDLE)
			{
				// check here if this swap is good!
				// if it can, animation has finished, and Remove()
				// will be called on the removable gems from AlgorithmHelper

				AlgorithmHelper::SwapJewels(m_selectedJewel, m_secondJewel, m_board);
				// as we've swopped them, reset droptargets!
				m_selectedJewel->ResetDropTarget();
				m_secondJewel->ResetDropTarget();

				if (AlgorithmHelper::ProcessMatch3(m_board) > 0)
				{
					swapAnimationStep = SWAP_STEP_END + 1;
				}
				else
				{
					// physically swap them back (instantly)
					AlgorithmHelper::SwapJewels(m_selectedJewel, m_secondJewel, m_board);
				}
			}

			if (swapAnimationStep > SWAP_STEP_END)
			{
				// swap them back!
				// end
				swapAnimationStep = SWAP_STEP_START;
				movementSpeed = STEP_PIXEL_COUNT[0];
				m_bPerformingSwap = false;
				m_selectedJewel->SetSelected(false);
				m_selectedJewel = NULL;
				m_secondJewel = NULL;
				m_selectionCursor->SetVisibility(false);
			}
			else
			{
				movementSpeed = STEP_PIXEL_COUNT[swapAnimationStep];
				swapAnimationStep++;

				//printf("%d", movementSpeed );
				switch (m_swapType)
				{
				case SWAP_TYPE_ABOVE:
					m_selectedJewel->SetPosRelative(0, -movementSpeed);
					m_secondJewel->SetPosRelative(0, movementSpeed);
					break;
				case SWAP_TYPE_BELOW:
					m_selectedJewel->SetPosRelative(0, movementSpeed);
					m_secondJewel->SetPosRelative(0, -movementSpeed);
					break;
				case SWAP_TYPE_LEFT:
					m_selectedJewel->SetPosRelative(-movementSpeed, 0);
					m_secondJewel->SetPosRelative(movementSpeed, 0);
					break;
				case SWAP_TYPE_RIGHT:
					m_selectedJewel->SetPosRelative(movementSpeed, 0);
					m_secondJewel->SetPosRelative(-movementSpeed, 0);
					break;
				case SWAP_TYPE_NONE:
					// m_performingSwap == true, m_swapType == SWAP_TYPE_NONE?!
					assert(0);
					break;
				}
			}
		}
		else
		{
			for (int i = BOARD_SIZE - 1; i >= 0; --i)
			{
				for (int j = BOARD_SIZE - 1; j >= 0; --j)
				{
					m_board[j][i]->Update();
					if (m_board[j][i]->IsDead())
					{
						AlgorithmHelper::RemoveAndSpawn(m_board[j][i], m_board, m_numNewJewelsThisCascadePerColumn[j]);
						m_numNewJewelsThisCascadePerColumn[j]++;
						m_bJewelsInMotion = true;

						j++;//	this now refers to a new jewel, so this will need reprocessing!
					}
					else if (m_board[j][i]->IsDropping())
					{
						m_bJewelsInMotion = true;
					}
					else if (m_board[j][i]->IsSelected())
					{
						m_selectedJewel = (AOJewel*)m_board[j][i];
					}
				}
			}
		}
	}

	// To allow 1-time-checking for cascades
	static bool doCheck = false;

	// If jewels are currently animating/cascading/dropping, 
	if (m_bJewelsInMotion)
	{
		// we should plan to do a single check when they have next stopped.		
		doCheck = true;
	}

	// If the jewels are not moving and we have a 'check' to do,
	if (!m_bJewelsInMotion && doCheck)
	{
		// Fetch the number of new matches (if any)
		int newMatches = AlgorithmHelper::ProcessMatch3(m_board);
		// Note that ProcessMatch3 will also set all matched
		//  jewels to begin the 'Remove' animation

		// If matches have been found (cascade occuring)
		if (newMatches > 0)
		{
			// increment our current cascade amount by the number
			//  of matches (becomes a 'multiplier')
			m_cascadeAmount += newMatches;
			// play sound - we have just found a new match!
			SDLAudio::Get()->PlaySFX("GotSet", 1.0f);
			SDLAudio::Get()->PlaySFX("Combo2", 1.0f);
		}
		else
		{
			// cascade has ended
			m_cascadeAmount = 0;
		}

		// As this is a one-time operation, we can crowbar this in here to 
		//  update the number of moves available
		m_numMovesAvailable = AlgorithmHelper::CountAvailableMoves(m_board);

		for (int i = 0; i < BOARD_SIZE; ++i)
		{
			m_numNewJewelsThisCascadePerColumn[i] = 0;
		}

		// Remind ourselves that we do not need to redo this check - it will 
		//  only occur again AFTER some jewel has begun moving. This ensures 
		//  that we only process these expensive algorithms once as necessary.
		doCheck = false;
	}

	// If we have run out of moves
	if (m_numMovesAvailable == 0)
	{
		// Generate a new board
		AlgorithmHelper::GenerateNewBoard(m_board, true);
		// and update the number of available moves
		m_numMovesAvailable = AlgorithmHelper::CountAvailableMoves(m_board);
	}

	// To allow 1-time checking for cursor presence
	static bool cursorPresent = false;
	// If we have not got a jewel selected, but we have the cursor present,
	if (m_selectedJewel == NULL && cursorPresent)
	{
		// Reset and hide the cursor
		m_selectionCursor->SetVisibility(false);
		m_selectionCursor->SetSwapAnimationFlag(false);
		cursorPresent = false;
	}
	// However, if we have got a jewel selected, and we don't yet have the
	//  cursor present,
	if (m_selectedJewel != NULL && !cursorPresent)
	{
		// Ensure that the cursor shall get drawn
		m_selectionCursor->SetVisibility(true);
		// and update the position it should be drawn at
		m_selectionCursor->SetPos(m_selectedJewel->GetXPos(), m_selectedJewel->GetYPos());
		cursorPresent = true;
	}

	// If we are performing a swap, we need to keep the selection cursor
	//  constantly updated,
	if (m_bPerformingSwap)
	{
		assert(m_secondJewel != NULL);
		// announce that we are performing a swap, and need a mirrored 
		//  selection cursor
		m_selectionCursor->SetSwapAnimationFlag(true);
		// tell the selection cursor the latest pos to draw at when rendering
		m_selectionCursor->SetPos(m_selectedJewel->GetXPos(), m_selectedJewel->GetYPos());
		// tell the selection cursor the latest pos to mirror at when rendering
		m_selectionCursor->SetMirrorPos(m_secondJewel->GetXPos(), m_secondJewel->GetYPos());
	}

	m_hint->Update();
	m_newGame->Update();
	m_newGameNormal->Update();
	m_newGameTimed->Update();
	m_options->Update();
	m_quit->Update();

	return 0;
}

int GameBoard::Render()
{
	// base chequered board is at the back
	SDLRenderer::Get()->DrawTexture(m_nextBackground, MAIN_BOARD_OFFSET_X - JEWEL_SPACING, MAIN_BOARD_OFFSET_Y - JEWEL_SPACING, 512, 512);

	// then (onboard) text or jewels
	if (m_bPauseEnabled)
	{
		SDLRenderer::Get()->DrawStr("Default", MAIN_BOARD_OFFSET_X + 92, MAIN_BOARD_OFFSET_Y + 180, "*** GAME PAUSED ***", 0xffffffff);
		SDLRenderer::Get()->DrawStr("Default", MAIN_BOARD_OFFSET_X + 78, MAIN_BOARD_OFFSET_Y + 210, "CLICK HERE TO CONTINUE!", 0xffffffff);
	}
	else
	{
		for (int i = 0; i < BOARD_SIZE; ++i)
		{
			for (int j = 0; j < BOARD_SIZE; ++j)
			{
				if (!m_bHideGems)
				{
					m_board[j][i]->Render();
				}
			}
		}
		m_selectionCursor->Render();

		m_hint->Render();
		m_newGame->Render();
		m_newGameNormal->Render();
		m_newGameTimed->Render();
		m_options->Render();
		m_quit->Render();
	}

	// finally, enclose the gameboard with the board sprite (note alphas!)
	SDLRenderer::Get()->DrawTexture(m_windowSpriteID, 0, 0, 640, 480);

	return 0;
}

void GetNewColourAdjustmentValues(float& r, float& g, float& b)
{
	// Background colours set here are designed to match Bejeweled exactly.
	const float TILE_ADJ_COLS[][3] = {
	 { 0.42f, 0.33f, 0.26f }, // brown
	 { 0.95f, 0.6f,  0.5f  }, // pink/salmon
	 { 0.35f, 0.35f, 0.7f  }, // blue
	 { 0.8f,  0.8f,  0.8f  }, // grey
	 { 0.95f, 0.8f,  0.4f  }, // gold
	 { 0.3f,  0.7f,  0.3f  }, // green (emerald)
	 //{ 1.0f,  1.0f,  1.0f  }, // white
	};

	const unsigned int totalCols = sizeof(TILE_ADJ_COLS) / (3 * sizeof(float));

	static int count = 0; // Temp to cycle through colours
	if (count >= totalCols) count = 0;
	int retVal = count++;

	r = TILE_ADJ_COLS[retVal][0];
	g = TILE_ADJ_COLS[retVal][1];
	b = TILE_ADJ_COLS[retVal][2];
}

bool GameBoard::GenerateNextTileBackground(const unsigned int level)
{
	// level should be 1-5
	// loop after 5, reduce our 'looped level' count to start at '0'
	int loopedLevel = (level - 1) % 5;

	SDL_Texture* pTileTextureSheet = SDLRenderer::Get()->GetTexture(m_backgroundTileSheetID);

	SDLRenderer::Get()->GenerateTiledTexture(m_nextBackground, pTileTextureSheet, loopedLevel, (loopedLevel * 2) + 1);

	// Adjust the tiled background colour
	static float r, g, b;
	
	if (!m_bNoRandCol)
	{
		GetNewColourAdjustmentValues(r, g, b);
	}
	
	SDL_SetTextureColorMod(m_nextBackground, static_cast<Uint8>(r * 255), static_cast<Uint8>(g * 255), static_cast<Uint8>(b * 255));

	return true;
}

AOJewel* GameBoard::GetJewelAt(const int& x, const int& y)
{
	//	HACK! Reverse-lookup jewel location from cursor pos

	const static Rect boardRect = { MAIN_BOARD_OFFSET_X, MAIN_BOARD_OFFSET_Y,
								   MAIN_BOARD_OFFSET_X + (JEWEL_SPACING * BOARD_SIZE),
								   MAIN_BOARD_OFFSET_Y + (JEWEL_SPACING * BOARD_SIZE) };

	if (x < boardRect.left || x >= boardRect.right ||
		y < boardRect.top || y >= boardRect.bottom)
	{
		return nullptr;
	}
	else
	{
		// we SHOULD be able to rely on this!

		// x & y are inside the board:
		int localX = x - MAIN_BOARD_OFFSET_X;
		int localY = y - MAIN_BOARD_OFFSET_Y;
		int jewelX = (int)floor((float)localX / (float)JEWEL_SPACING);
		int jewelY = (int)floor((float)localY / (float)JEWEL_SPACING);

		//printf("Found jewel: %d, %d\n", jewelX, jewelY);

		assert(jewelX < BOARD_SIZE);
		assert(jewelY < BOARD_SIZE);

		return m_board[jewelX][jewelY];
	}
}

bool AreJewelsAdjacent(AOJewel* jewel1, AOJewel* jewel2, SWAP_TYPE& swapType)
{
	// This finds out whether two jewels are adjacent to eachother (swappable)
	// if they are, it outputs true (otherwise false)
	// on true, it also sets swapType to the type of swap that should take place
	// (above, below, left or right), from the perspective of the first jewel.

	VEC2 pos1(jewel1->GetXPos(), jewel1->GetYPos());
	VEC2 pos2(jewel2->GetXPos(), jewel2->GetYPos());

	unsigned short col1 = (unsigned short)floor((float)pos1.x / (float)JEWEL_SPACING);
	unsigned short col2 = (unsigned short)floor((float)pos2.x / (float)JEWEL_SPACING);
	unsigned short row1 = (unsigned short)floor((float)pos1.y / (float)JEWEL_SPACING);
	unsigned short row2 = (unsigned short)floor((float)pos2.y / (float)JEWEL_SPACING);

	if (col1 == col2)
	{
		if (row1 == row2 + 1 || row1 == row2 - 1)
		{
			//second jewel is above/below first jewel
			if (row1 == row2 + 1)
			{
				swapType = SWAP_TYPE_ABOVE;
			}
			else
			{
				swapType = SWAP_TYPE_BELOW;
			}
			return true;
		}
	}
	else if (row1 == row2)
	{
		if (col1 == col2 + 1 || col1 == col2 - 1)
		{
			if (col1 == col2 + 1)
			{
				swapType = SWAP_TYPE_LEFT;
			}
			else
			{
				swapType = SWAP_TYPE_RIGHT;
			}
			//second jewel is left/right of first jewel
			return true;
		}
	}

	return false;
}
bool GameBoard::HandleMouseClickUpAt(const int& x, const int& y)
{
	m_newGame->HandleMouseClickUpAt(x, y);
	m_newGameNormal->HandleMouseClickUpAt(x, y);
	m_newGameTimed->HandleMouseClickUpAt(x, y);
	m_options->HandleMouseClickUpAt(x, y);
	m_hint->HandleMouseClickUpAt(x, y);
	m_quit->HandleMouseClickUpAt(x, y);

	return true;
}

bool GameBoard::HandleMouseClickDownAt(const int& x, const int& y)
{
	Rect br = GetBoardRect();

	if (m_bJewelsInMotion || m_bPerformingSwap)
		return true;

	if (m_bPauseEnabled)
	{
		// Click within gameboard when paused - should unpause!
		if (x > br.left && x < br.right && y > br.top && y < br.bottom)
			m_bPauseEnabled = false;
	}
	else
	{
		// if busy? Do nothing.
		if (m_bPerformingSwap || m_bJewelsInMotion)
			return true;

		// standard click
		if (x > br.left && x < br.right && y > br.top && y < br.bottom)
		{
			// within board
			AOJewel* nextJewel = GetJewelAt(x, y);
			if (nextJewel != NULL)
			{
				// on a jewel
				if (!nextJewel->IsSelected())
				{
					// that is not yet selected
					if (m_selectedJewel != NULL)
					{
						// and we have already previously selected another jewel
						if (AreJewelsAdjacent(m_selectedJewel, nextJewel,
							m_swapType))
						{
							// and these jewels are adjacent

							// init. attempt swap of these jewels
							printf("**Attempting swap of jewels "
								"at (%d,%d) and (%d,%d)!**\n",
								m_selectedJewel->GetXPos(),
								m_selectedJewel->GetYPos(),
								nextJewel->GetXPos(),
								nextJewel->GetYPos());

							m_secondJewel = nextJewel;

							m_bPerformingSwap = true;
						}
						else
						{
							// Attempting to select two non-adjacent jewels
							//  ...Bejeweled behaviour is to unselect
							//      currently selected jewel
							printf("Second jewel is NOT adjacent. Cancelling!");
							m_selectedJewel->SetSelected(false);
							m_selectedJewel = NULL;
						}
					}
					else
					{
						// no jewel is selected, select this jewel!
						printf("New jewel selected! [%d]\n", (int)nextJewel->GetJewelType());
						nextJewel->SetSelected(true);
						m_selectedJewel = nextJewel;
						SDLAudio::Get()->PlaySFX("Select", 1.0f);
					}
				}
				else
				{
					printf("Jewel was already selected. Cancelling.\n");
					// jewel was already selected
					nextJewel->HandleMouseClickDownAt(x, y);
					// clicking on an already selected jewel...
					m_selectedJewel = NULL;
				}
			}
		}
	}

	m_newGame->HandleMouseClickDownAt(x, y);
	m_newGameNormal->HandleMouseClickDownAt(x, y);
	m_newGameTimed->HandleMouseClickDownAt(x, y);
	m_options->HandleMouseClickDownAt(x, y);
	m_hint->HandleMouseClickDownAt(x, y);
	m_quit->HandleMouseClickDownAt(x, y);

	return true;
}

bool GameBoard::HandleMouseHoverAt(const int& x, const int& y)
{
	AOJewel* pJewel = GetJewelAt(x, y);
	if (pJewel != nullptr && !m_bPauseEnabled)
	{
		pJewel->HandleMouseHoverAt(x, y);
	}


	// do not show hand cursor by default
	MouseCursor::Get()->SetShowHand(false);
	// if we find that in the following functions it should be a hand,
	//  then change it to a hand cursor
	m_newGame->HandleMouseHoverAt(x, y);
	m_newGameNormal->HandleMouseHoverAt(x, y);
	m_newGameTimed->HandleMouseHoverAt(x, y);
	m_options->HandleMouseHoverAt(x, y);
	m_hint->HandleMouseHoverAt(x, y);
	m_quit->HandleMouseHoverAt(x, y);

	return true;
}

bool GameBoard::EngagePauseMode()
{
	return (m_bPauseEnabled = true);
}
bool GameBoard::ResumeFromPauseMode()
{
	return (m_bPauseEnabled = false);
}

Rect GameBoard::GetBoardRect()
{
	Rect retVal = { MAIN_BOARD_OFFSET_X,
					MAIN_BOARD_OFFSET_Y,
					MAIN_BOARD_OFFSET_X + (JEWEL_SPACING * 8),
					MAIN_BOARD_OFFSET_Y + (JEWEL_SPACING * 8) };

	return retVal;
}
#ifdef _DEBUG
void GameBoard::DebugDump()
{
	printf("\nDumping Board!\n");

	printf("\nJewels:\n\n");
	printf("       x  y\n");
	printf("      ======\n");

	for (int i = 0; i < BOARD_SIZE; ++i)
	{
		for (int j = 0; j < BOARD_SIZE; ++j)
		{
			printf("Jewel [%d][%d] ", j, i);
			m_board[j][i]->DebugDump();
		}
	}

	printf("\nButtons:TODO\n\n");
	//TODO dump button info
	printf("\nOther:TODO\n\n");
	//TODO dump all other info (playerscore?)
	printf("\nBoard Dumped!\n");
}
#endif //_DEBUG
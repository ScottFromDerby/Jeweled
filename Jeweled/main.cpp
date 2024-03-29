#include <stdio.h>
#include <assert.h>
#include <time.h>

#include "AlgorithmHelper.h"
#include "AOSelectionCursor.h"
#include "AOPressStartBtn.h"
#include "GameBoard.h"
#include "MouseCursor.h"

#include "SDL.h"
#include "SDLAudio.h"
#include "SDLRenderer.h"
#include "SDL_image.h"

//#include <shlobj_core.h>

extern "C" { FILE __iob_func[3] = { *stdin,*stdout,*stderr }; }

const unsigned int WINDOW_WIDTH = 640;//468;
const unsigned int WINDOW_HEIGHT = 480;//342;
const bool WINDOW_FULLSCREEN = false;
const int DEFAULT_FONT_SIZE = 24;
const char* ICON_FILE_NAME = "..\\Resources\\Bejeweled1.bmp";
const char* WINDOW_TITLE = "Jeweled - Immense Games";
const unsigned int FRAME_RATE = 80; // Capped frame rate!
const float GAME_DEFAULT_VOLUME = 0.8f;

#define LOG(x) printf("%s\n", (x));
#define ASSERT(x) assert(x)

// The board - contains all jewel objects
GameBoard* g_pGameBoard = nullptr;

AOPressStartBtn* g_pPressStartBtn = nullptr;

// current framerate
//double g_currentFrameRate;

// Gamestate object
enum GAME_STATE
{
	GS_NONE = 0,
	GS_SPLASH,
	GS_INGAME,
	GS_PAUSED
} g_currentGameState;

int init()
{
	LOG("Init()");

	//	Load/Init Renderer
	SDLRenderer* pRenderer = SDLRenderer::Get();
	pRenderer->Init(WINDOW_WIDTH, WINDOW_HEIGHT, WINDOW_FULLSCREEN, WINDOW_TITLE, ICON_FILE_NAME);

	pRenderer->ParseFontData("..\\Resources\\afont1.txt", "AFont1");
	pRenderer->ParseFontData("..\\Resources\\mainfont1.txt", "MainFont");
	pRenderer->ParseFontData("..\\Resources\\scorefont1.txt", "ScoreFont");
	pRenderer->ParseFontData("..\\Resources\\alphadigits4.txt", "ScorePopupFont");
	pRenderer->ParseFontData("..\\Resources\\overfont2.txt", "OverFont");

	//	Load in some shared board sprites
	pRenderer->AddImage("..\\Resources\\titlescreen6.jpg", "TitleScreen");
	pRenderer->AddImage("..\\Resources\\alphaback1.tga", "WindowBase");
	pRenderer->AddImage("..\\Resources\\checkertextures2.png", "BackgroundTileSheet");
	pRenderer->AddImage("..\\Resources\\target1.tga", "SelectionCursor");
	pRenderer->AddImage("..\\Resources\\JewelSheet.tga", "JewelSpriteSheet");
	pRenderer->AddImage("..\\Resources\\dialogbox360-200.tga", "DialogBox");
	pRenderer->AddImage("..\\Resources\\_afont1.gif", "AFont1");
	pRenderer->AddImage("..\\Resources\\_mainfont1.gif", "MainFont");
	pRenderer->AddImage("..\\Resources\\scorefont1.png", "ScoreFont");
	pRenderer->AddImage("..\\Resources\\alphadigits4.png", "ScorePopupFont");
	pRenderer->AddImage("..\\Resources\\overfont2.png", "OverFont");
	pRenderer->AddImage("..\\Resources\\timerbar.png", "ScoreBar");
	
	

	//	Load/Init GameBoard object
	g_pGameBoard = new GameBoard("WindowBase", "JewelSpriteSheet", "BackgroundTileSheet");
	g_pGameBoard->Init();

	g_pPressStartBtn = new AOPressStartBtn(VEC2(212, 387));

	//	Load/Init Mouse Cursor
	MouseCursor::Get()->Init();

	//	Load/Init SDLAudio
	SDLAudio::Get()->Init();
	SDLAudio::Get()->SetMusic("Music1", GAME_DEFAULT_VOLUME);

	//	Seed random number generator
	time_t now;
	time(&now);
	srand((int)now);

	//	Setup initial variables
	g_currentGameState = GS_SPLASH;

	return 0;
}

int update()
{
	switch (g_currentGameState)
	{
	case GS_SPLASH:
		break;
	case GS_INGAME:
		g_pGameBoard->Update();
		break;
	default:
		assert(0); // gamestate unhandled
		break;
	}

	return 0;
}

int render()
{
	static SDLRenderer* pRenderer = SDLRenderer::Get();

	pRenderer->StartDrawing();

	switch (g_currentGameState)
	{
	case GS_SPLASH:
		pRenderer->DrawTexture("TitleScreen", 0, 0, 640, 480);
		g_pPressStartBtn->Render();
		//pRenderer->DrawStr("AFont1", 208, 380, "CLICK HERE TO PLAY!", 0xff00ff00);
		pRenderer->DrawStr("MainFont", 490, 210, "Version 1.xx", 0xff888888);
		//pRenderer->DrawStr("ScoreFont", 490, 310, "0123456", 0xffffffff);
		//pRenderer->DrawStr("ScorePopupFont", 390, 410, "+50+100+150", 0xffffffff);
		//pRenderer->DrawStr("OverFont", 290, 110, "GAME OVER", 0xffffffff);
		break;
	case GS_INGAME:
		g_pGameBoard->Render();
		break;
	default:
		assert(0); // gamestate unhandled
		break;
	}

	// Finally draw cursor on top of everything
	MouseCursor::Get()->Render();

	pRenderer->EndDrawing();

	return 0;
}

int SDL_main(int argc, char* argv[])
{
	//	****** Minimum rendering testbed ******
	//SDL_Init(SDL_INIT_VIDEO);
	//IMG_Init(IMG_INIT_JPG | IMG_INIT_PNG);
	//
	//SDL_Window* pWnd = SDL_CreateWindow("Test", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 640, 480, SDL_WINDOW_MOUSE_FOCUS | SDL_WINDOW_OPENGL);
	//SDL_Renderer* pRenderer = SDL_CreateRenderer(pWnd, -1, 0);
	//
	////SDL_Surface* pSurface = IMG_Load("..\\Resources\\titlescreen6.jpg");
	////SDL_Surface* pSurface = IMG_Load("..\\Resources\\_afont1.gif");
	////SDL_SetColorKey(pSurface, SDL_TRUE, 15);
	//
	//SDL_Texture* pMainBackground = SDL_CreateTexture(pRenderer, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_TARGET, 640, 480);
	//
	//SDL_Surface* pSurface = IMG_Load("..\\Resources\\checkertextures2.tga");
	//SDL_Texture* pTileTextureSheet = SDL_CreateTextureFromSurface(pRenderer, pSurface);
	//
	//SDL_Rect src1 = { 0, 0, JEWEL_SIZE, JEWEL_SIZE };
	//SDL_Rect src2 = { JEWEL_SIZE, 0, JEWEL_SIZE, JEWEL_SIZE };
	//
	//SDL_Texture* pTile1 = SDL_CreateTexture(pRenderer, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_TARGET, JEWEL_SIZE, JEWEL_SIZE);
	//SDL_SetRenderTarget(pRenderer, pTile1);
	//SDL_RenderCopy(pRenderer, pTileTextureSheet, &src1, nullptr);
	//
	//SDL_Texture* pTile2 = SDL_CreateTexture(pRenderer, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_TARGET, JEWEL_SIZE, JEWEL_SIZE);
	//SDL_SetRenderTarget(pRenderer, pTile2);
	//SDL_RenderCopy(pRenderer, pTileTextureSheet, &src2, nullptr);
	//
	//SDL_SetRenderTarget(pRenderer, nullptr);
	//
	////	Copy squares 
	////int iWidth1 = 0, iHeight1 = 0, iWidth2 = 0, iHeight2 = 0;
	////SDL_QueryTexture(pTile1, nullptr, nullptr, &iWidth1, &iHeight1);
	////SDL_QueryTexture(pTile2, nullptr, nullptr, &iWidth2, &iHeight2);
	////assert(iWidth1 == iWidth2);
	////assert(iWidth2 == iHeight2);
	////
	////int iTargetWidth = 0, iTargetHeight = 0;
	////SDL_QueryTexture(pMainBackground, nullptr, nullptr, &iTargetWidth, &iTargetHeight);
	////
	////const Sint16 xReps = (Sint16)floor((double)(iTargetWidth / iWidth1)) + 1;
	////const Sint16 yReps = (Sint16)floor((double)(iTargetHeight / iHeight1)) + 1;
	////
	////SDL_SetRenderTarget(pRenderer, pMainBackground);
	////for (Sint16 i = 0; i < xReps; ++i)
	////{
	////	for (Sint16 j = 0; j < yReps; ++j)
	////	{
	////		SDL_Rect destRect = { static_cast<Sint16>(i * iWidth1), static_cast<Sint16>(j * iHeight1), static_cast<Uint16>(iWidth1), static_cast<Uint16>(iHeight1) };
	////		SDL_RenderCopy(pRenderer, ((i + j) % 2 == 0) ? pTile1 : pTile2, nullptr, &destRect);
	////	}
	////}
	////SDL_SetRenderTarget(pRenderer, nullptr);
	//
	////SDL_Texture* pTexture = SDL_CreateTextureFromSurface(pRenderer, pSurface);
	//while(true)
	//{
	//	SDL_PumpEvents();
	//
	//	SDL_SetRenderTarget(pRenderer, nullptr);
	//
	//	SDL_SetRenderDrawColor(pRenderer, 255, 0, 255, 127);
	//	SDL_RenderClear(pRenderer);
	//
	//	//
	//	//SDL_Rect rcDest = { 0, 0, pSurface->w, pSurface->h};
	//	//SDL_RenderCopy(pRenderer, pTexture, nullptr, &rcDest);
	//	//SDL_RenderCopy(pRenderer, pTile1, nullptr, nullptr);
	//
	//	SDL_RenderCopy(pRenderer, pMainBackground, nullptr, nullptr);
	//
	//	SDL_RenderPresent(pRenderer);
	//
	//	SDL_Delay(50);
	//}





	//	TODO: Attempt load resources from Bejeweled Deluxe Demo
	//FILE* pFile = nullptr;
	//fopen_s(&pFile, "Resources.txt", "rw");
	//if(pFile == nullptr)
	//{
	//	SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_WARNING, "Resources not yet found", "Cannot find local Jeweled resources, please point me to an installation of Bejeweled�", nullptr);
	//
	//	BROWSEINFO br;
	//	ZeroMemory(&br, sizeof(BROWSEINFO));
	//	br.lpfn = BrowseCallbackProc;
	//	br.ulFlags = BIF_RETURNONLYFSDIRS | BIF_NEWDIALOGSTYLE;
	//	br.hwndOwner = hwnd;
	//	br.lpszTitle = title;
	//	br.lParam = (LPARAM)folder.GetString();
	//
	//	LPITEMIDLIST pidl = NULL;
	//	if ((pidl = SHBrowseForFolder(&br)) != NULL)
	//	{
	//		wchar_t buffer[MAX_PATH];
	//		if (SHGetPathFromIDList(pidl, buffer)) ret = buffer;
	//	}
	//}


	init();

	int g_uLastUpdateTicks = 0; // in ticks

	while (true)
	{
		if (update() == -1)
		{
			return 1;
		}

		if (render() == -1)
		{
			return 2;
		}

		SDL_PumpEvents();

		const Uint32 WindowFlags = SDL_GetWindowFlags(SDLRenderer::Get()->GetMainWindow());

		// Check for window messages (e.g. clicking [X])
		SDL_Event ev;
		while (SDL_PollEvent(&ev))
		{
			switch (ev.type)
			{
			case SDL_QUIT:
				return 0;
			case SDL_KEYDOWN:
			{
				static int level = 0;
				if (ev.key.keysym.sym == SDLK_n)
				{
					g_pGameBoard->GenerateNextTileBackground(level++);
				}
				else if (ev.key.keysym.sym == SDLK_m)
				{
					g_pGameBoard->GenerateNextTileBackground(level);
				}
				else if (ev.key.keysym.sym == SDLK_b)
				{
					g_pGameBoard->m_bNoRandCol = !g_pGameBoard->m_bNoRandCol;
				}
				else if (ev.key.keysym.sym == SDLK_h)
				{
					g_pGameBoard->m_bHideGems = !g_pGameBoard->m_bHideGems;
				}
				else if (ev.key.keysym.sym == SDLK_p)
				{
					if (g_pGameBoard->IsPaused())
					{
						g_pGameBoard->ResumeFromPauseMode();
					}
					else
					{
						g_pGameBoard->EngagePauseMode();
					}
				}
				else if (ev.key.keysym.sym == SDLK_c)
				{
					printf("\n\n\n");
					g_pGameBoard->CountAvailableMoves();
					printf("\n\n\n");
				}
				else if (ev.key.keysym.sym == SDLK_r)
				{
					//	reset
					printf("\nReset!!\n\n");
					g_pGameBoard->ResetBoard(true);
				}
#ifdef _DEBUG
				else if (ev.key.keysym.sym == SDLK_d)
				{
					//	dump
					printf("Debug Dump!\n");
					g_pGameBoard->DebugDump();
				}
#endif //_DEBUG

				break;
			}
			case SDL_MOUSEBUTTONDOWN:
			{
				if (g_currentGameState != GS_SPLASH)
				{
					g_pGameBoard->HandleMouseClickDownAt(ev.button.x, ev.button.y);
				}

				break;
			}
			case SDL_MOUSEBUTTONUP:
			{
				//	##SB move all this to gamestate handler
				if (g_currentGameState == GS_SPLASH)
				{
					g_pPressStartBtn->HandleMouseClickUpAt(ev.button.x, ev.button.y);
					if( g_pPressStartBtn->IsSelected() )
					{
						g_currentGameState = GS_INGAME;
						SDLAudio::Get()->SetMusic("Music3", GAME_DEFAULT_VOLUME);
						SDLAudio::Get()->PlaySFX("Go", GAME_DEFAULT_VOLUME);
					}
				}
				else
				{
					g_pGameBoard->HandleMouseClickUpAt(ev.button.x, ev.button.y);
				}

				break;
			}
			case SDL_MOUSEMOTION:
			{
				if (g_currentGameState == GS_INGAME)
				{
					if (WindowFlags & SDL_WINDOW_INPUT_FOCUS)
					{
						g_pGameBoard->HandleMouseHoverAt(ev.motion.x, ev.motion.y);
					}

					if (SDL_GetMouseState(nullptr, nullptr) & SDL_MOUSEBUTTONDOWN)
					{
						g_pGameBoard->HandleMouseDragAt(ev.motion.x, ev.motion.y);
					}
				}
				else if( g_currentGameState == GS_SPLASH )
				{
					g_pPressStartBtn->HandleMouseHoverAt(ev.motion.x, ev.motion.y);
				}
			}
			default:
				break;
			}
		}

		// If the mouse is hovering anywhere in the window, and we are in a 
		//  game process any potential mouse hovering.
		if (g_currentGameState == GS_INGAME)
		{
			if (WindowFlags & SDL_WINDOW_INPUT_FOCUS)
			{
				int mouseX, mouseY; SDL_GetMouseState(&mouseX, &mouseY);
				g_pGameBoard->HandleMouseHoverAt(mouseX, mouseY);
				// log mouse location
				//printf("\r%d, %d     ", mouseX, mouseY);
			}
		}

		// Check to see whether app deserves focus or not, and shift
		//  accordingly if necessary
		static bool appHasFocus = false;
		if (!(WindowFlags & SDL_WINDOW_INPUT_FOCUS) && appHasFocus)
		{
			//printf("App DOES NOT HAVE FOCUS...\n");
			//g_gameBoard->EngagePauseMode();
			SDLAudio::Get()->SetGlobalVolume(0.0f);
			appHasFocus = false;
		}
		if ((WindowFlags & SDL_WINDOW_INPUT_FOCUS) && !appHasFocus)
		{
			//printf("App SHOULD HAVE FOCUS...\n");
			SDLAudio::Get()->SetGlobalVolume(GAME_DEFAULT_VOLUME);
			appHasFocus = true;
		}

		do { SDL_Delay(1); } while (SDL_GetTicks() - g_uLastUpdateTicks < (1000 / FRAME_RATE));

		g_uLastUpdateTicks = SDL_GetTicks();

		// FPS Counter
		static Uint32 uLastUpdateForFPS = 0;
		static Uint32 uNumFramesPassed = 0;
		uNumFramesPassed++;
		if ((g_uLastUpdateTicks - uLastUpdateForFPS) > 1000)
		{
			//g_currentFrameRate = (float)uNumFramesPassed;
			uNumFramesPassed = 0;
			uLastUpdateForFPS = g_uLastUpdateTicks;
		}
	}

	SDLRenderer::Get()->Shutdown();

	return 0;
}
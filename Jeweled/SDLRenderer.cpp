#include "SDLRenderer.h"
#include "main.h"

#include "SDL.h"
#include "SDL_ttf.h"
#include "SDL_image.h"

#include <math.h>
#include <assert.h>
#include <windows.h>
#include <map>
#include <vector>

////////////////////////////////////////////////////
// SDLRenderer
//  A simple *generic* SDL Renderer class
//  This provides all the core functionality for
//  rendering to the screen, managing textures,
//  drawing text and geometry, and generating a
//  tiled SDLSurface texture from input textures.
////////////////////////////////////////////////////

SDLRenderer::SDLRenderer()
 :	m_pMainWindow(nullptr),
	m_pRenderer(nullptr)
{
}

void SDLRenderer::Shutdown()
{
	for (std::pair<std::string, SDL_Surface*> Surface : m_surfaceManager)
	{
		SDL_FreeSurface(Surface.second);
	}
	m_surfaceManager.clear();
	for (std::pair<std::string, SDL_Texture*> Texture : m_textureManager)
	{
		SDL_DestroyTexture(Texture.second);
	}
	m_textureManager.clear();
}

SDLRenderer* SDLRenderer::Get()
{
	// Singleton pattern
	static SDLRenderer* pInst = new SDLRenderer();
	return pInst;
}

SDL_Texture* SDLRenderer::GetTexture(const std::string& spriteID)
{ 
	SDL_Texture* pTexture = m_textureManager[spriteID];
	SDL_assert(pTexture != nullptr);
	return pTexture;
}

int SDLRenderer::Init(int width, int height, bool bFullscreen, const std::string& windowTitle, const std::string& iconFile)
{
	printf("SDLRenderer::Init()\n");
	// Initialize SDL's subsystems - video
	if (SDL_Init(SDL_INIT_VIDEO) < 0)
	{
		fprintf(stderr, "Unable to init SDL: %s\n", SDL_GetError());
		return -1;
	}

	if (IMG_Init(IMG_INIT_JPG | IMG_INIT_PNG) < 0)
	{
		fprintf(stderr, "Unable to init SDL_image: %s\n", SDL_GetError());
		return -2;
	}

	Uint32 windowFlags = SDL_WINDOW_MOUSE_FOCUS | SDL_WINDOW_OPENGL;
	if (bFullscreen)
	{
		windowFlags |= SDL_WINDOW_FULLSCREEN;
	}
	//SDL_WINDOWPOS_UNDEFINED
	m_pMainWindow = SDL_CreateWindow(windowTitle.c_str(), SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, width, height, windowFlags);
	assert(m_pMainWindow != nullptr);

	m_pRenderer = SDL_CreateRenderer(m_pMainWindow, -1, 0);
	assert(m_pRenderer != nullptr);
	
	m_pFontRenderingBufferTexture = SDL_CreateTexture(m_pRenderer, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_STREAMING, 1024, 1024);

	SDL_SetWindowIcon(m_pMainWindow, IMG_Load(iconFile.c_str()));

	return 0;
}

void SDLRenderer::StartDrawing()
{
	//	Start of every frame!
	SDL_SetRenderDrawColor(m_pRenderer, 255, 0, 255, 255);
	SDL_RenderClear(m_pRenderer);
}

void SDLRenderer::EndDrawing()
{
	//	End of every frame!

	//SDL_GL_SetSwapInterval(1);
	SDL_RenderPresent(m_pRenderer);
}

void SDLRenderer::DrawStr(const std::string& fontID, Sint16 x, Sint16 y, const std::string& msg, unsigned int colour)
{
	SDL_Texture* pFontTexture = m_textureManager[fontID];
	FontOffsetMap& Offsets = m_fontXOffsets[fontID];

	int xPos = x;

	SDL_SetTextureColorMod(pFontTexture, (colour >> 16) & 0xff, (colour >> 8) & 0xff, colour & 0xff );
	SDL_SetTextureAlphaMod(pFontTexture, 255);

	int fontHeight = m_fontHeights[fontID];

	for (char c : msg)
	{
		std::map<char, FontOffsetData>::const_iterator iter = Offsets.find(c);
		if (iter != Offsets.end())
		{
			const FontOffsetData& data = iter->second;
			
			if(data.uXOffset >= 0 )
			{
				SDL_Rect rcDest = { xPos, y, static_cast<Uint16>(data.uWidth), fontHeight };
				SDL_Rect rcSource = { data.uXOffset, 0, static_cast<Uint16>(data.uWidth), fontHeight };
				SDL_RenderCopy(m_pRenderer, pFontTexture, &rcSource, &rcDest);
			}
			xPos += data.uWidth;
		}
	}

	//if (SDL_Surface* pTextSurface = TTF_RenderText_Solid(font, msg.c_str(), c))
	//{
	//	SDL_Rect rcDest = { x, y, static_cast<Uint16>(pTextSurface->clip_rect.w), static_cast<Uint16>(pTextSurface->clip_rect.h) }; 
	//	
	//	SDL_Texture* pTexture = SDL_CreateTextureFromSurface(m_pRenderer, pTextSurface);
	//	SDL_RenderCopy(m_pRenderer, pTexture, nullptr, &rcDest);
	//	SDL_DestroyTexture(pTexture);
	// 
	//	//SDL_Surface* pWritableSurface = nullptr;
	//	//SDL_LockTextureToSurface(m_pFontRenderingBufferTexture, nullptr, &pWritableSurface);
	//	//if(pWritableSurface != nullptr)
	//	//{
	//	//	SDL_BlitSurface(pTextSurface, nullptr, pWritableSurface, nullptr);
	//	//	SDL_UnlockSurface(pWritableSurface);
	//	//}
	//	//
	//	//SDL_RenderCopy(m_pRenderer, m_pFontRenderingBufferTexture, nullptr, &rcDest);
	//
	//	SDL_FreeSurface(pTextSurface); 
	//}
}

unsigned int SDLRenderer::AddImage(const std::string& imagePath, const std::string& refName)
{
	SDL_assert(m_pRenderer != nullptr);

	printf("SDLRenderer::AddImage(%s)\n", refName.c_str());
	SDL_Surface* pNewSurface = IMG_Load(imagePath.c_str());
	assert(pNewSurface);

	if( refName.compare("AFont1") == 0 )
	{
		SDL_SetColorKey(pNewSurface, SDL_TRUE, 15);
		m_fontHeights["AFont1"] = pNewSurface->h;
	} else if (refName.compare("MainFont") == 0)
	{
		SDL_SetColorKey(pNewSurface, SDL_TRUE, 15);
		m_fontHeights["MainFont"] = pNewSurface->h;
	}

	m_surfaceManager[refName] = pNewSurface;
	m_textureManager[refName] = SDL_CreateTextureFromSurface(m_pRenderer, pNewSurface);
	
	// returning size-1 here in order to correlate with image ID
	//  (which starts at 0), as size() starts at 1.
	return (unsigned int)m_surfaceManager.size() - 1;
}

void SDLRenderer::DrawTexture(const std::string& surfaceName, Sint16 iDestX, Sint16 iDestY, Uint16 uWidth, Uint16 uHeight)
{
	DrawTexture(m_textureManager[surfaceName], iDestX, iDestY, uWidth, uHeight);
}

void SDLRenderer::DrawTexture(SDL_Texture* pTex, Sint16 iDestX, Sint16 iDestY, Uint16 uWidth, Uint16 uHeight)
{
	SDL_Rect rcDest = { iDestX, iDestY, uWidth, uHeight };
	SDL_RenderCopy(m_pRenderer, pTex, nullptr, &rcDest);
}

void SDLRenderer::DrawPartialTexture(const std::string& surfaceID, SDL_Rect* rcSrc, Sint16 iDestX, Sint16 iDestY, Uint16 uWidth, Uint16 uHeight)
{
	DrawPartialTexture(m_textureManager[surfaceID], rcSrc, iDestX, iDestY, uWidth, uHeight);
}

void SDLRenderer::DrawPartialTexture(SDL_Texture* pTex, SDL_Rect* rcSrc, Sint16 iDestX, Sint16 iDestY, Uint16 uWidth, Uint16 uHeight)
{
	SDL_Rect rcDest = { iDestX, iDestY, uWidth, uHeight };
	SDL_RenderCopy(m_pRenderer, pTex, rcSrc, &rcDest);
}

void SDLRenderer::GenerateTiledTexture(SDL_Texture* pTextureOut, SDL_Texture* pTileSheet, int index1, int index2)
{
	SDL_SetRenderTarget(m_pRenderer, pTextureOut);

	int iTargetWidth = 0, iTargetHeight = 0, iTilesheetWidth = 0, iTilesheetHeight = 0;
	SDL_QueryTexture(pTextureOut, nullptr, nullptr, &iTargetWidth, &iTargetHeight);
	SDL_QueryTexture(pTileSheet, nullptr, nullptr, &iTilesheetWidth, &iTilesheetHeight);

	const Sint16 xReps = (Sint16)floor((double)(iTargetWidth / iTilesheetHeight)) + 1;
	const Sint16 yReps = (Sint16)floor((double)(iTargetHeight / iTilesheetHeight)) + 1;
	
	SDL_Rect rcTile1 = { 0, 0, iTilesheetHeight, iTilesheetHeight };
	SDL_Rect rcTile2 = { iTilesheetHeight, 0, iTilesheetHeight, iTilesheetHeight };
	
	for (Sint16 i = 0; i < xReps; ++i)
	{
		for (Sint16 j = 0; j < yReps; ++j)
		{
			SDL_Rect destRect = { i * iTilesheetHeight, j * iTilesheetHeight, iTilesheetHeight, iTilesheetHeight };
			SDL_RenderCopy(m_pRenderer, pTileSheet,((i + j) % 2 == 0) ? &rcTile1: &rcTile2, &destRect);
		}
	}

	SDL_SetRenderTarget(m_pRenderer, nullptr);
}

void SDLRenderer::GenerateTiledTexture(SDL_Texture* pTextureOut, SDL_Texture* pInTile1, SDL_Texture* pInTile2)
{
	//	Double checking!
	int iWidth1 = 0, iHeight1 = 0, iWidth2 = 0, iHeight2 = 0;
	SDL_QueryTexture(pInTile1, nullptr, nullptr, &iWidth1, &iHeight1);
	SDL_QueryTexture(pInTile2, nullptr, nullptr, &iWidth2, &iHeight2);
	assert(iWidth1 == iWidth2);
	assert(iWidth2 == iHeight2);
	// assuming identically sized objects have been passed
	// assuming out is a prepared SDL_Surface* object

	//assert(in1->w == in2->w && in1->h == in2->h);

	int iTargetWidth = 0, iTargetHeight = 0;
	SDL_QueryTexture(pTextureOut, nullptr, nullptr, &iTargetWidth, &iTargetHeight);

	const Sint16 xReps = (Sint16)floor((double)(iTargetWidth / iWidth1)) + 1;
	const Sint16 yReps = (Sint16)floor((double)(iTargetHeight / iHeight1)) + 1;

	for (Sint16 i = 0; i < xReps; ++i)
	{
		for (Sint16 j = 0; j < yReps; ++j)
		{
			SDL_Rect destRect = { static_cast<Sint16>(i * iWidth1), static_cast<Sint16>(j * iHeight1), static_cast<Uint16>(iWidth1), static_cast<Uint16>(iHeight1) };
			SDL_RenderCopy(m_pRenderer, ((i + j) % 2 == 0) ? pInTile1 : pInTile2, nullptr, &destRect);
		}
	}
}

void SDLRenderer::AdjustPixels(SDL_Surface* out, float rAdj, float gAdj, float bAdj)
{
	// Note: intensive operation - do NOT use live/every frame!

	for (int i = 0; i < out->h * (out->pitch / 4); ++i)
	{
		unsigned int pixel = ((unsigned int*)out->pixels)[i];

		BYTE a, r, g, b;

		a = pixel >> 24;
		r = pixel >> 16;
		g = pixel >> 8;
		b = pixel >> 0;

		//	perform effect
		r = (BYTE)((Uint16)r * rAdj);
		g = (BYTE)((Uint16)g * gAdj);
		b = (BYTE)((Uint16)b * bAdj);

		pixel = (a << 24) + (r << 16) + (g << 8) + (b);
		((unsigned int*)out->pixels)[i] = pixel;
	}
}

//void SDLRenderer::AddFont(const std::string& fontRef, const std::string& fontFilename, int pointSize)
//{
//	//printf("SDLRenderer::AddFont(%s)\n", fontRef.c_str());
//	//TTF_Font* pNewFont = TTF_OpenFont(fontFilename.c_str(), pointSize);
//	//if (pNewFont == nullptr)
//	//{
//	//	MessageBox(nullptr, TEXT("Could not load font!\r\nCheck resources folder!"), SDL_GetError(), MB_ICONERROR);
//	//	SDL_Quit();
//	//	return;
//	//}
//
//	//m_fontManager[fontRef] = pNewFont;
//}

void SDLRenderer::ParseFontData(const char* FontDataFile, const std::string& fontID)
{
	char* buffer = nullptr;

	FILE* pFile = nullptr;
	fopen_s(&pFile, FontDataFile, "rb");
	if (pFile != nullptr)
	{
		fseek(pFile, 0, SEEK_END);
		long fileSize = ftell(pFile);
		rewind(pFile);

		buffer = new char[fileSize];

		fread(buffer, fileSize, 1, pFile);
		fclose(pFile);

		std::vector<char> CharList;
		std::vector<long> WidthList;

		char* pIter = &buffer[0];
		while(*pIter != '\0')
		{
			const char* STR_CHARLIST = "Define CharList";
			const char* STR_WIDTHLIST = "Define WidthList";
			const char* STR_LAYERSETCHARWIDTHS = "LayerSetCharWidths";

			if (strncmp(STR_CHARLIST, pIter, strlen(STR_CHARLIST) - 1) == 0)
			//if (strcmpi("Define CharList\r\n", pIter) == 0)
			{
				pIter += strlen(STR_CHARLIST);

				//	Pull in chars to parse
				char nextChar = *pIter;
				while (nextChar != ';' && nextChar != '\0')
				{
					if (nextChar == '\'' || nextChar == '\"')
					{
						//	Expect symbol next
						char foundChar = *++pIter;
						CharList.push_back(foundChar);

						nextChar = *++pIter;	//	Advance past closing '
						//assert(nextChar == '\'' || nextChar == '\"');
						if( nextChar == '\\' )
						{
							nextChar = *++pIter;	//	Advance past closing '
						}
					}
					nextChar = *++pIter;
				}
			}

			if (strncmp(STR_WIDTHLIST, pIter, strlen(STR_WIDTHLIST) - 1) == 0)
			{
				pIter += strlen(STR_WIDTHLIST);

				//	Pull in chars to parse
				char nextChar = *pIter;
				while (nextChar != ';' && nextChar != '\0')
				{
					if (nextChar >= '0' && nextChar <= '9')
					{
						//	Reading a numeral
						long value = strtol(pIter, &pIter, 10);
						WidthList.push_back(value);
					}
					nextChar = *++pIter;
				}
			}

			if (strncmp(STR_LAYERSETCHARWIDTHS, pIter, strlen(STR_LAYERSETCHARWIDTHS) - 1) == 0)
			{
				pIter += strlen(STR_LAYERSETCHARWIDTHS);

				char nextChar = *pIter;
				while (nextChar != ';' && nextChar != '\0')
				{
					if (nextChar >= '0' && nextChar <= '9')
					{
						//	Reading a numeral
						long value = strtol(pIter, &pIter, 10);

						//	This is the space character
						CharList.push_back(' ');
						WidthList.push_back(value);

						nextChar = *++pIter;	//	Advance past closing '
					}

					nextChar = *++pIter;
				}
			}
			
			++pIter;
		}

		assert(CharList.size() == WidthList.size());

		FontOffsetMap& OffsetMap = m_fontXOffsets[fontID];

		int iOffset = 0;
		for (size_t i = 0; i < CharList.size(); ++i)
		{
			char nextChar = CharList[i];
			OffsetMap[nextChar].uWidth = static_cast<Uint16>(WidthList[i]);
			OffsetMap[nextChar].uXOffset = nextChar == ' ' ? -1 : iOffset;
			iOffset += WidthList[i];
		}

		//m_mainFontXOffsets[' '].uWidth = 10;
		//m_mainFontXOffsets[' '].uXOffset = 0;

		delete[](buffer);
	}
}

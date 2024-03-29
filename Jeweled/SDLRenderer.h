#pragma once

#include <SDL.h>
#include <SDL_ttf.h>

#include <map>
#include <string>

class SDLRenderer
{
private:
	SDLRenderer();

public:
	//	Singleton
	static SDLRenderer* Get();

public:
	int Init(int width, int height, bool bFullscreen, const std::string& windowTitle, const std::string& iconFile);
	void Shutdown();

	bool AddImage(const std::string& imagePath, const std::string& refName);
	SDL_Texture* GetTexture(const std::string& spriteID);

	void StartDrawing();
	void EndDrawing();

	void DrawStr(const std::string& fontID, Sint16 x, Sint16 y, const std::string& msg, unsigned int colour);

	void DrawRect(const SDL_Rect* rc, Uint8 r, Uint8 g, Uint8 b, Uint8 a);
	void DrawLine(int x1, int y1, int x2, int y2, Uint8 r, Uint8 g, Uint8 b, Uint8 a);
	void DrawFilledRect(const SDL_Rect* rc, Uint8 r, Uint8 g, Uint8 b, Uint8 a);

	void DrawTexture(const std::string& surfaceID, Sint16 iDestX, Sint16 iDestY, Uint16 uWidth, Uint16 uHeight);
	void DrawTexture(SDL_Texture* pTex, Sint16 iDestX, Sint16 iDestY, Uint16 uWidth, Uint16 uHeight);
	
	void DrawPartialTexture(const std::string& surfaceID, SDL_Rect* rcSrc, Sint16 iDestX, Sint16 iDestY, Uint16 uWidth, Uint16 uHeight);
	void DrawPartialTexture(SDL_Texture* pTex, SDL_Rect* rcSrc, Sint16 iDestX, Sint16 iDestY, Uint16 uWidth, Uint16 uHeight);

	void GenerateTiledTexture(SDL_Texture* pTextureOut, SDL_Texture* pTileSheet, int index1, int index2);
	void GenerateTiledTexture(SDL_Texture* pTextureOut, SDL_Texture* pInTile1, SDL_Texture* pInTile2);

	static void AdjustPixels(SDL_Surface* out, float rAdj, float gAdj, float bAdj);

	SDL_Window* GetMainWindow()							{ return m_pMainWindow; }
	SDL_Renderer* GetRenderer()							{ return m_pRenderer; }
	int GetFontHeight(const std::string& fontID) const;

	int GetCharWidthPx(const std::string& fontID, char wantedChar) const;
	int CalculateStringWidth(const std::string& fontID, const char* buffer) const;

	void ParseFontData(const char* FontDataFile, const std::string& fontID);
	void ParseComplexFont(char* buffer, const std::string& fontID);
	void ParseSimpleFont(char* buffer, const std::string& fontID);

private:
	SDL_Window* m_pMainWindow;
	SDL_Renderer* m_pRenderer;

	SDL_Texture* m_pFontRenderingBufferTexture;

	struct FontOffsetData
	{
		Sint16 uXOffset;
		Uint16 uWidth;
	};

	typedef std::map<char, FontOffsetData> FontOffsetMap;

	std::map<std::string, FontOffsetMap> m_fontXOffsets;
	std::map<std::string, int> m_fontHeights;

	std::map<std::string, SDL_Texture*> m_textureManager;
	std::map<std::string, SDL_Surface*> m_surfaceManager;
};

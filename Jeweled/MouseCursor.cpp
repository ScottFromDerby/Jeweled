#include "MouseCursor.h"
#include "SDLRenderer.h"
#include "SDL.h"


////////////////////////////////////////////////////
// MouseCursor
// 
// This basic class allows the app to override
// the standard cursor with a sprite-based cursor.
////////////////////////////////////////////////////

MouseCursor::MouseCursor()
	: m_bIsHand(false)
	, m_size(54, 54)
{
}

MouseCursor* MouseCursor::Get()
{
	static MouseCursor* pInst = new MouseCursor();
	return pInst;
}

void MouseCursor::Init()
{
	// Load sprites
	// NB. There is only ever one MouseCursor, and only ever one object that
	//  will ever use these sprites. For the sake of simplicity, and as they 
	//  need not exist outside of this class, they are loaded here.
	SDLRenderer::Get()->AddImage("..\\Resources\\cursor1.tga", "MouseCursor");
	SDLRenderer::Get()->AddImage("..\\Resources\\pointer2.tga", "MouseHand");

	// Hides the standard mouse cursor from view while inside this window.
	SDL_ShowCursor(0);
}

void MouseCursor::Render()
{
	int x, y;
	SDL_GetMouseState(&x, &y);

	SDLRenderer::Get()->DrawTexture(m_bIsHand ? "MouseHand" : "MouseCursor", x + MOUSE_CURSOR_OFFSET_X, y + MOUSE_CURSOR_OFFSET_Y, m_size.x, m_size.y);
}

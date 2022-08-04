#include "AOSelectionCursor.h"
#include "SDLRenderer.h"

////////////////////////////////////////////////////
// AOSelectionCursor
//  This class represents the image shown when the
//  player has selected a jewel. It has been
//  extended to render a mirror of itself when a
//  swap is taking place, as Bejeweled does.
////////////////////////////////////////////////////

AOSelectionCursor::AOSelectionCursor(const VEC2& size, const std::string& spriteID) :
	ActiveObject(VEC2_Zero, size, spriteID),
	m_animationFrame(0),
	m_performingSwap(false),
	m_altPos(VEC2_Zero)
{
}

int AOSelectionCursor::Init()
{
	SetVisibility(false);
	return 0;
}

int AOSelectionCursor::Update()
{
	return 0;
}

int AOSelectionCursor::Render()
{
	const unsigned short ANIMATION_FRAMES = 6;
	const unsigned short ANIMATION_DELAY = 5;

	if (m_bIsVisible)
	{
		m_animationFrame++;
		if (m_animationFrame >= ANIMATION_FRAMES * ANIMATION_DELAY)
		{
			m_animationFrame = 0;
		}

		SDL_Rect partialSpriteRect = { static_cast<Sint16>((int)(m_animationFrame / ANIMATION_DELAY) * m_size.x), 0, static_cast<Uint16>(m_size.x), static_cast<Uint16>(m_size.y) };
		SDLRenderer::Get()->DrawPartialTexture(m_spriteIDBasic, &partialSpriteRect, m_pos.x, m_pos.y, m_size.x, m_size.y);

		//	if we are performing a swap
		if (m_performingSwap)
		{
			//	also draw the selection cursor on the jewel we are swapping with!
			SDLRenderer::Get()->DrawPartialTexture(m_spriteIDBasic, &partialSpriteRect, m_altPos.x, m_altPos.y, m_size.x, m_size.y);
		}
	}

	return 0;
}

bool AOSelectionCursor::PerformSwapAnimation(const VEC2& destPos)
{
	m_altPos = destPos;
	return true;
}

void AOSelectionCursor::SetMirrorPos(int x, int y)
{
	m_altPos.x = x;
	m_altPos.y = y;
}
#include "AOPressStartBtn.h"

#include <assert.h>

#include "AlgorithmHelper.h"
#include "MouseCursor.h"
#include "SDLRenderer.h"
#include "SDLAudio.h"


AOPressStartBtn::AOPressStartBtn(const VEC2& pos)
	: ActiveObject(pos, VEC2(215, 24))
{
}

bool AOPressStartBtn::HandleMouseClickUpAt(int x, int y)
{
	if (GetClickableArea().Contains(x, y))
	{
		SetSelected(true);
	}
	return false;
}

bool AOPressStartBtn::HandleMouseHoverAt(int x, int y)
{
	m_bHovered = GetClickableArea().Contains(x, y);

	MouseCursor::Get()->SetShowHand(m_bHovered);
	return false;
}

int AOPressStartBtn::Render()
{
	int col = m_bHovered ? 0xffffffff : 0xff00ff00;

	SDLRenderer::Get()->DrawStr("AFont1", m_pos.x, m_pos.y, "CLICK HERE TO PLAY!", col);

	SDLRenderer::Get()->DrawLine(m_pos.x, m_pos.y + 22, m_pos.x + m_size.x, m_pos.y + 22, 
		col & 0xff, (col >> 8) & 0xff, (col >> 16) & 0xff, (col >> 24) & 0xff);

	//SDL_Rect rc = { m_pos.x, m_pos.y, m_size.x, m_size.y };
	//SDLRenderer::Get()->DrawRect(&rc, 255, 0, 255, 255);

	return 0;
}

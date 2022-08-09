#include "AOScorePopup.h"
#include "SDLRenderer.h"
#include "AlgorithmHelper.h"
#include "SDLAudio.h"
#include <assert.h>


AOScorePopup::AOScorePopup(const VEC2& pos, int points)
	: ActiveObject(pos, VEC2(128,128))
{
	sprintf_s(buffer, "%d", points);
}

int AOScorePopup::Update()
{
	m_fLifeRemaining -= 1.0f / 60.0f;	//	TBD: improve, frame-dependent!!
	m_fHeightOffset -= 0.5f;

	return 0;
}

int AOScorePopup::Render()
{
	Sint16 xPos = m_pos.x;
	Sint16 yPos = m_pos.y;

	//	Center
	xPos -= SDLRenderer::Get()->CalculateStringWidth("ScorePopupFont", buffer) / 2;
	yPos -= SDLRenderer::Get()->GetFontHeight("ScorePopupFont") / 2;

	yPos += (int)m_fHeightOffset;

	int alpha = m_fLifeRemaining > 0.5f ? 255 : (int)(m_fLifeRemaining * 512);

	SDLRenderer::Get()->DrawStr("ScorePopupFont", xPos, yPos, buffer, 0x00ffffff | ((alpha & 0xff) << 24));

	return 0;
}

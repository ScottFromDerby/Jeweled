#include "AOScoreBar.h"
#include "SDLRenderer.h"
#include "AlgorithmHelper.h"
#include "SDLAudio.h"
#include <assert.h>

AOScoreBar::AOScoreBar(const VEC2& pos, const VEC2& size)
	: ActiveObject(pos, size)
{
}

int AOScoreBar::Init()
{
	m_nNextLevelTargetScore = 500;
	m_nActualScore = 0;
	return 0;
}

int AOScoreBar::Update()
{
	//m_nActualScore++;
	return 0;
}

int AOScoreBar::Render()
{
	float pctToFill = Max(m_nActualScore / (float)m_nNextLevelTargetScore, 1.0f);

	Uint16 fillWidth = (Uint16)((float)m_size.x * pctToFill);

	SDL_Rect rcSrc = { 0, 0, fillWidth, m_size.y };

	SDL_SetTextureColorMod(SDLRenderer::Get()->GetTexture("ScoreBar"), 0, 255, 0);
	SDLRenderer::Get()->DrawPartialTexture("ScoreBar", &rcSrc, m_pos.x, m_pos.y, fillWidth, m_size.y);

	//	TODO: This isn't right. Result is too black, not blended properly
	//SDLRenderer::Get()->DrawStr("AFont1", 602, 446, "1", 0xff000000);
	SDL_SetTextureBlendMode(SDLRenderer::Get()->GetTexture("AFont1"), SDL_BLENDMODE_BLEND);
	SDLRenderer::Get()->DrawStr("AFont1", 602, 446, "1", 0xffffffff);

	return 0;
}

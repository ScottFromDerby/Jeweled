#include "AOScore.h"
#include "SDLRenderer.h"
#include "AlgorithmHelper.h"
#include "SDLAudio.h"
#include <assert.h>

AOScore::AOScore(const VEC2& pos)
	: ActiveObject(pos, VEC2_Zero)
{
}

int AOScore::Init()
{
	m_nInProgressScoreCount = 0;
	m_nActualScore = 0;
	return 0;
}

int AOScore::Update()
{
	//m_nActualScore++;

	if (m_nInProgressScoreCount < m_nActualScore)
	{
		m_nInProgressScoreCount++;
	}
	if ((m_nInProgressScoreCount * 10) < m_nActualScore)
	{
		m_nInProgressScoreCount += 10;
	}
	return 0;
}

int AOScore::Render()
{
	char buffer[128];
	sprintf_s(buffer, "%d", m_nInProgressScoreCount);

	//	Faux-centering
	int numChars = strlen(buffer);

	int xPos = m_pos.x - (int)(numChars * (SDLRenderer::Get()->GetCharWidthPx("ScoreFont", '0') * 0.5f));

	SDLRenderer::Get()->DrawStr("ScoreFont", xPos, m_pos.y, buffer, 0xffffffff);

	return 0;
}

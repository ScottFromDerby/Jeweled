#pragma once

#include "ActiveObject.h"
#include "GameBoard.h"


class AOScorePopup : public ActiveObject
{
public:
	AOScorePopup(const VEC2& pos, int points);

public:
	virtual int Init() { return 0; }
	virtual int Update();
	virtual int Render();

	bool ShouldKill() const	 { return m_fLifeRemaining < 0.0f; }

	//	overloading pure virtual base functions
	const char* GetType()		{ return "AOScorePopup"; }

private:
	char buffer[16];
	float m_fLifeRemaining = 1.3f;
	float m_fHeightOffset = 0.0f;
};
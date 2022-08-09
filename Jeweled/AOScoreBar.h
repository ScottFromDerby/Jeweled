#pragma once

#include "ActiveObject.h"
#include "GameBoard.h"

class AOScoreBar : public ActiveObject
{
public:
	AOScoreBar(const VEC2& pos, const VEC2& size);

public:
	virtual int Init() override;
	virtual int Update() override;
	virtual int Render() override;

	void SetScore(int points)		{ m_nActualScore = points; }
	//void AddToScore(int points)		{ m_nActualScore += points; }

	//	Pure virtual
	const char* GetType()			{ return "AOScore"; }

private:
	int m_nNextLevelTargetScore = 500;
	int m_nActualScore = 0;
};

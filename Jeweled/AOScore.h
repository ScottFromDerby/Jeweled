#pragma once

#include "ActiveObject.h"
#include "GameBoard.h"

class AOScore : public ActiveObject
{
public:
	AOScore(const VEC2& pos);

public:
	virtual int Init() override;
	virtual int Update() override;
	virtual int Render() override;

	int GetScore() const			{ return m_nActualScore; }
	void AddToScore(int points)		{ m_nActualScore += points; }

	//	Pure virtual
	const char* GetType()			{ return "AOScore"; }

private:
	int m_nInProgressScoreCount = 0;
	int m_nActualScore = 0;
};

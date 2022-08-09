#pragma once

#include "ActiveObject.h"
#include "GameBoard.h"

class AOPressStartBtn : public ActiveObject
{
public:
	AOPressStartBtn(const VEC2& pos);

public:
	virtual int Init() override { return 0; }
	virtual int Update() override { return 0; }
	virtual int Render() override;
	virtual bool HandleMouseClickUpAt(int x, int y) override;
	virtual bool HandleMouseHoverAt(int x, int y) override;

	//	Pure virtual
	const char* GetType()			{ return "AOPressStartBtn"; }

public:
	bool m_bHovered = false;
};

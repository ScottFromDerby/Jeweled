#pragma once
#include "ActiveObject.h"
#include <assert.h>

class AOSelectionCursor : public ActiveObject
{
public:
	AOSelectionCursor(const VEC2& size, const std::string& spriteID);
	
public:
	virtual int Init();
	virtual int Update();
	virtual int Render();

	const char* GetType() { return "AOSelectionCursor"; }

	virtual bool HandleMouseClickDownAt(int x, int y)	{ assert(0); return false; }
	virtual bool HandleMouseClickUpAt(int x, int y)		{ assert(0); return false; }
	virtual bool HandleMouseHoverAt(int x, int y)		{ assert(0); return false; }

	void SetMirrorPos(int x, int y);

	bool PerformSwapAnimation(const VEC2& destPos);

	bool IsPerformingSwapAnimation() { return m_performingSwap; }
	void SetSwapAnimationFlag(bool newState) { m_performingSwap = newState; }

private:
	unsigned short m_animationFrame;
	bool m_performingSwap;
	VEC2 m_altPos; // for use when performing a swap animation
};

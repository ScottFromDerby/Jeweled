#pragma once

#include "SDL.h"
#include "ActiveObject.h"

class MouseCursor
{
private:
	MouseCursor();

public:
	static MouseCursor* Get();

public:
	void Init();
	void Render();
	void SetShowHand(bool bShowHand)		{ m_bIsHand = bShowHand; }

private:
	bool m_bIsHand;
	VEC2 m_size;
	
	// These are used to offset the mouse cursor
	//  to the sprites' logical hot-spot location
	static const int MOUSE_CURSOR_OFFSET_X = -26;
	static const int MOUSE_CURSOR_OFFSET_Y = -28;
};

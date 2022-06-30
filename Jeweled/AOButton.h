#pragma once
#include "ActiveObject.h"

enum BUTTON_TYPE {  BUTTON_TYPE_NEWGAME,
					BUTTON_TYPE_NG_NORMAL,
					BUTTON_TYPE_NG_TIMED,
					BUTTON_TYPE_OPTIONS, 
					BUTTON_TYPE_QUIT, 
					BUTTON_TYPE_HINT };

class AOButton : public ActiveObject
{
public:
	AOButton(const VEC2& pos, const VEC2& size, const std::string& spriteRef, const std::string& spriteHoverRef, const std::string& spriteClickedRef, const std::string& spriteSelectedRef, BUTTON_TYPE buttonType);

public:
	virtual int Init(); // pure virtual
	virtual int Update(); // pure virtual
	virtual int Render(); // pure virtual
public:
	virtual bool PerformClickedFunctionality(); // should be overloaded!
	
	virtual bool HandleMouseClickDownAt(int x, int y);
	virtual bool HandleMouseClickUpAt(int x, int y);
	virtual bool HandleMouseHoverAt(int x, int y);
	virtual const char* GetType() { return "AOButton"; }

	bool SetSize( int x, int y );

private:
	bool m_pushed; // different to selected, pushed is being clicked right now.
	bool m_hoveredOver; // if this is currently being hovered over.
	BUTTON_TYPE m_buttonType; // specifies a button behaviour.
};

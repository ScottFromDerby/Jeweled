#pragma once
#include "ActiveObject.h"

class AODialog : public ActiveObject
{
public:
	AODialog( VEC2 pos, VEC2 size, const char* spriteRef,
				const char* spriteHoverRef, const char* spriteClickedRef,
				const char* spriteSelectedRef );
	~AODialog(void);
public:
	virtual int Init();
	virtual int Update();
	virtual int Render();

	void SetText( char* newText ) { m_messageText = newText; }

public:
	virtual const char* GetType() { return "AODialog"; }
	virtual bool HandleMouseClickDownAt(int x, int y);
	virtual bool HandleMouseClickUpAt(int x, int y);
	virtual bool HandleMouseHoverAt(int x, int y);

	char* m_messageText;
};

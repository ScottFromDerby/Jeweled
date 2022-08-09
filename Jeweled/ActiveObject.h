#pragma once

#include <string>

// Basic structs to carry data around

struct Rect
{
	int left;
	int top;
	int right;
	int bottom;

	bool Contains(int x, int y)
	{
		return x >= left && x < right && y >= top && y < bottom;
	}
};

class VEC2
{
public:
	VEC2() : x(0), y(0) {}
	VEC2(const VEC2& other) : x(other.x), y(other.y) {}
	VEC2(int nX, int nY) : x(nX), y(nY) {}

public:
	int x;
	int y;
};

extern const VEC2 VEC2_Zero;

class ActiveObject
{
protected: // do not directly instantiate base objs!
	ActiveObject(const VEC2& pos, const VEC2& size, const std::string& spriteID = "", const std::string& spriteIDHover = "", const std::string& spriteIDClicked = "", const std::string& spriteIDSelected = "");

public:
	virtual int Init() = 0; // pure virtual
	virtual int Update() = 0; // pure virtual
	virtual int Render() = 0; // pure virtual

public:
	virtual const std::string& GetDefaultSpriteID()		{ return m_spriteIDBasic; }

	virtual long GetXPos() { return m_pos.x; }
	virtual long GetYPos() { return m_pos.y; }
	virtual inline Rect GetClickableArea();

	virtual bool SetPos(int x, int y);
	virtual bool SetPosRelative(int x, int y);

	virtual bool IsClickable() { return m_bIsClickable; };
	virtual bool IsSelected() { return m_bIsSelected; };
	virtual bool IsVisible() { return m_bIsVisible; };
	virtual void SetClickable(bool clickable) { m_bIsClickable = clickable; };
	virtual void SetSelected(bool selected) { m_bIsSelected = selected; };
	virtual void SetVisibility(bool visible) { m_bIsVisible = visible; };

	virtual const char* GetType() = 0; // pure virtual
	virtual bool HandleMouseClickDownAt(int x, int y)	{ return false; }
	virtual bool HandleMouseClickUpAt(int x, int y)		{ return false; }
	virtual bool HandleMouseHoverAt(int x, int y)		{ return false; }

protected:
	// Active Object properties
	VEC2 m_pos;
	Rect m_relativeClickableArea;
	VEC2 m_size;

	bool m_bIsClickable;
	bool m_bIsSelected;
	bool m_bIsVisible;

	std::string m_spriteIDBasic;
	std::string m_spriteIDHover;
	std::string m_spriteIDClicked; //pushed
	std::string m_spriteIDSelected;
};
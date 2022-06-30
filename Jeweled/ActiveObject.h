#pragma once

#include <string>

// Basic structs to carry data around

struct Rect
{
	int left;
	int top;
	int right;
	int bottom;
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
	//virtual void SetDefaultSpriteSheetID(int i) { m_spriteIDBasic = i; }

	virtual long GetXPos() { return m_pos.x; }
	virtual long GetYPos() { return m_pos.y; }
	virtual inline Rect GetClickableArea();

	virtual bool SetPos(int x, int y);
	virtual bool SetPosRelative(int x, int y);

	virtual bool IsClickable() { return m_isClickable; };
	virtual bool IsSelected() { return m_isSelected; };
	virtual bool IsVisible() { return m_isVisible; };
	virtual void SetClickable(bool clickable) { m_isClickable = clickable; };
	virtual void SetSelected(bool selected) { m_isSelected = selected; };
	virtual void SetVisibility(bool visible) { m_isVisible = visible; };

	virtual const char* GetType() = 0; // pure virtual
	virtual bool HandleMouseClickDownAt(int x, int y) = 0; // pure virtual
	virtual bool HandleMouseClickUpAt(int x, int y) = 0; // pure virtual
	virtual bool HandleMouseHoverAt(int x, int y) = 0; // pure virtual

protected:
	// Active Object properties
	VEC2 m_pos;
	Rect m_relativeClickableArea;
	VEC2 m_size;

	bool m_isClickable;
	bool m_isSelected;
	bool m_isVisible;

	std::string m_spriteIDBasic;
	std::string m_spriteIDHover;
	std::string m_spriteIDClicked; //pushed
	std::string m_spriteIDSelected;
};
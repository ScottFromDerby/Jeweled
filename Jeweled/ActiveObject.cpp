#include "ActiveObject.h"
#include "SDLRenderer.h"

////////////////////////////////////////////////////
// ActiveObject
//  This abstract base class defines the template
//  functionality for all ActiveObjects. All AO-
//  objects must inherit from the ActiveObject 
//  class, which provides some core functionality
//  which is shared across all active objects.
//  An ActiveObject here is a collection of sprites
//  which respond to a Mouse Click or Hover,
//  triggering respective functionality. It will
//  also have Init(), Update() and Render() functs.
////////////////////////////////////////////////////

const VEC2 VEC2_Zero = { 0, 0 };

ActiveObject::ActiveObject(const VEC2& pos, const VEC2& size, const std::string& spriteID, const std::string& spriteIDHover, const std::string& spriteIDClicked, const std::string& spriteIDSelected) :
	m_pos(pos),
	m_size(size),
	m_isClickable(false),
	m_isSelected(false),
	m_spriteIDBasic(spriteID),
	m_spriteIDHover(spriteIDHover),
	m_spriteIDClicked(spriteIDClicked),
	m_spriteIDSelected(spriteIDSelected)
{
	m_relativeClickableArea.left = 0;
	m_relativeClickableArea.top = 0;
	m_relativeClickableArea.right = size.x;
	m_relativeClickableArea.bottom = size.y;
}

Rect ActiveObject::GetClickableArea()
{
	// NB. returns Global coords, not local!
	Rect retVal;
	retVal.left = m_pos.x + m_relativeClickableArea.left;
	retVal.top = m_pos.y + m_relativeClickableArea.top;
	retVal.right = m_pos.x + m_relativeClickableArea.right;
	retVal.bottom = m_pos.y + m_relativeClickableArea.bottom;

	return retVal;
}

bool ActiveObject::SetPos(int x, int y)
{
	m_pos.x = x;
	m_pos.y = y;
	return true;
}

bool ActiveObject::SetPosRelative(int x, int y)
{
	SetPos(m_pos.x + x, m_pos.y + y);
	return true;
}
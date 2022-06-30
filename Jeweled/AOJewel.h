#pragma once

#include "ActiveObject.h"
#include "GameBoard.h"

class AOJewel : public ActiveObject
{
public:
	AOJewel(const VEC2& pos, const VEC2& size, unsigned short jewelType, const std::string& spriteSheetID);
	~AOJewel();

public:
	virtual int Init();
	virtual int Update();
	virtual int Render();

	bool IsHoveredOver() { return m_isInHoverState; }

	JEWEL_TYPE GetJewelType() { return (JEWEL_TYPE)m_jewelType; }
	void SetJewelType(JEWEL_TYPE nType) { m_jewelType = (unsigned short)nType; }

	bool HandleMouseClickDownAt(int x, int y);// overload the pure virtual funct
	bool HandleMouseClickUpAt(int x, int y); // overload the pure virtual funct
	bool HandleMouseHoverAt(int x, int y); // was pure virtual

	int GetDropTarget() { return m_pixelDropTarget; }

	void SetDropTarget(int newYPosTarget);
	void SetDropTargetRelative(int newYPosTarget);

	virtual bool SetPos(int x, int y);
	void ResetDropTarget();

	void Remove();

#ifdef _DEBUG
	void DebugDump();
#endif //_DEBUG

	bool IsDead()							{ return m_isDead; }
	void SetIsDead(bool newState)			{ m_isDead = newState; }

	bool IsDropping()						{ return m_isDropping; }
	void SetIsDropping(bool newState)		{ m_isDropping = newState; }

	//	overloading pure virtual base functions
	const char* GetType()		{ return "AOJewel"; }

private:
	//take note of inherited members!
	//bool m_isSelected; is an inherited member!

	unsigned short m_jewelType;		 // The type of jewel (0-7)
	unsigned short m_animationFrame; // The current frame of animation (or 0)
	int m_pixelDropTarget;			 // The target for this object to drop to
	float m_dropSpeed;				 // current dropping speed

	bool m_isInHoverState;			 // Is being hovered over
	bool m_isDropping;				 // Is currently dropping
	bool m_isBeingRemoved;			 // Is about to be removed
	bool m_isDead;					 // Jewel is dead - needs to be removed and
									 // respawned( will be handled within a
									 //  single Update loop).

	int m_beingRemovedOffset;
};

static const float DROP_SPEED_BASE = 1.5f;
static const float DROP_SPEED_MODIFIER = 1.08f;
static const float DROP_SPEED_MAX = 15.0f;
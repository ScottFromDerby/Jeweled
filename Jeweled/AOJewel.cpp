#include "AOJewel.h"
#include "SDLRenderer.h"
#include "AlgorithmHelper.h"
#include "SDLAudio.h"
#include <assert.h>

////////////////////////////////////////////////////
// AOJewel
//  An AOJewel represents a single jewel object in
//  the game. When hovered over it will begin a
//  'hover' animation, which stops when the 
//  animation is complete. This is not identical to
//  the Bejeweled implementation, but I think it
//  looks even better so I left it in. An AOJewel
//  also responds to clicks and is able to 'drop'
//  to a height target, as demonstrated at the start
//  of the game,
////////////////////////////////////////////////////

AOJewel::AOJewel(const VEC2& pos, const VEC2& size, unsigned short jewelType, const std::string& spriteSheetID)
	: ActiveObject(pos, size, spriteSheetID),
	m_jewelType(jewelType),
	m_animationFrame(0)
{
}

AOJewel::~AOJewel()
{
}

int AOJewel::Init()
{
	m_bIsInHoverState = false;
	m_bIsBeingRemoved = false;
	m_bIsDropping = false;
	m_pixelDropTarget = m_pos.y;
	m_bIsDead = false;
	m_dropSpeed = DROP_SPEED_BASE;
	m_beingRemovedOffset = 0;
	return 0;
}

int AOJewel::Update()
{
	// if it has finished animating
	if (m_animationFrame == 0)
	{
		m_bIsInHoverState = false;
	}

	// movement code for falling/animating
	if (m_bIsBeingRemoved)
	{
		m_beingRemovedOffset++;
		//m_pos.x += 1;
		//m_pos.y += 1;
		m_size.x -= 2;
		m_size.y -= 2;
		if (m_size.x <= 0)
		{
			m_beingRemovedOffset = 0;
			// finished animating - choose new jewel, move to above screen
			m_size.x = JEWEL_SIZE;
			m_size.y = JEWEL_SIZE;
			unsigned short newJewelType = (unsigned short)((float)(rand()) / RAND_MAX * NUM_JEWEL_TYPES);
			m_jewelType = (JEWEL_TYPE)newJewelType;
			m_bIsBeingRemoved = false;
			// set as dead so that the gameboard knows to remove and respawn
			//  this jewel.
			m_bIsDead = true;
		}
	}
	else if (m_bIsDropping)
	{
		if ((float)(m_pos.y) + m_dropSpeed >= (float)m_pixelDropTarget)
		{
			int x, y;
			AlgorithmHelper::EstimateJewelBoardLocation(this, x, y);
			m_pos.y = m_pixelDropTarget;
			printf("Jewel (likely:[%d][%d]) finished dropping at y:%d\n", x, y, m_pos.y);
			m_bIsDropping = false;
			m_dropSpeed = DROP_SPEED_BASE;
			SDLAudio::Get()->PlaySFX("GemOnGem", 0.3f);
		}
		else
		{
			assert(m_pixelDropTarget > m_pos.y);
			m_pos.y += (int)m_dropSpeed;
			m_dropSpeed *= DROP_SPEED_MODIFIER;
			if (m_dropSpeed > DROP_SPEED_MAX)
			{
				m_dropSpeed = DROP_SPEED_MAX;
			}

			//printf("Jewel [%d][%d] dropping at speed %1.3f\n", m_initialGridXPos, m_initialGridYPos, m_dropSpeed);
		}
	}

	return 0;
}

int AOJewel::Render()
{
	const unsigned short ANIMATION_FRAMES = 15;
	const unsigned short ANIMATION_DELAY = 2;

	if (m_bIsSelected) // SELECTED
	{
		//	starting pos in spritesheet is...
		const Sint16 SPRITE_SHEET_X = (int)(m_animationFrame / ANIMATION_DELAY) * JEWEL_SIZE;
		const Sint16 SPRITE_SHEET_Y = m_jewelType * (2 * JEWEL_SIZE);

		SDL_Rect srcRect = { SPRITE_SHEET_X, SPRITE_SHEET_Y, JEWEL_SIZE, JEWEL_SIZE };

		Sint16 x = m_pos.x + m_beingRemovedOffset;
		Sint16 y = m_pos.y + m_beingRemovedOffset;
		Uint16 w = m_size.x;
		Uint16 h = m_size.y;

		SDLRenderer::Get()->DrawPartialTexture(m_spriteIDBasic, &srcRect, x, y, w, h);

		m_animationFrame++;
		if (m_animationFrame >= ANIMATION_FRAMES * ANIMATION_DELAY)
		{
			m_animationFrame = 0;
		}
	}
	else if (m_bIsInHoverState) // HOVER
	{
		//	starting pos in spritesheet is...
		const Sint16 SPRITE_SHEET_X = (int)(m_animationFrame / ANIMATION_DELAY) * JEWEL_SIZE;
		const Sint16 SPRITE_SHEET_Y = (m_jewelType * (2 * JEWEL_SIZE)) + JEWEL_SIZE;

		SDL_Rect srcRect = { SPRITE_SHEET_X, SPRITE_SHEET_Y, JEWEL_SIZE, JEWEL_SIZE };

		Sint16 x = m_pos.x + m_beingRemovedOffset;
		Sint16 y = m_pos.y + m_beingRemovedOffset;
		Uint16 w = m_size.x;
		Uint16 h = m_size.y;

		SDLRenderer::Get()->DrawPartialTexture(m_spriteIDBasic, &srcRect, x, y, w, h);

		m_animationFrame++;
		if (m_animationFrame >= ANIMATION_FRAMES * ANIMATION_DELAY)
		{
			m_animationFrame = 0;
		}
	}
	else // NON-HOVERED, NON_SELECTED
	{
		//	standard render
		const Sint16 SPRITE_SHEET_X = 0;
		const Sint16 SPRITE_SHEET_Y = m_jewelType * (2 * JEWEL_SIZE);

		SDL_Rect srcRect = { SPRITE_SHEET_X, SPRITE_SHEET_Y, JEWEL_SIZE, JEWEL_SIZE };

		Sint16 x = m_pos.x + m_beingRemovedOffset;
		Sint16 y = m_pos.y + m_beingRemovedOffset;
		Uint16 w = m_size.x;
		Uint16 h = m_size.y;

		SDLRenderer::Get()->DrawPartialTexture(m_spriteIDBasic, &srcRect, x, y, w, h);

	}

	return 0;
}

bool AOJewel::HandleMouseClickDownAt(int x, int y)
{
	// by now, x & y should be within this jewel... but just to be sure:

	Rect thisJewelClickableArea = GetClickableArea();
	if (x > thisJewelClickableArea.left && x < thisJewelClickableArea.right &&
		y > thisJewelClickableArea.top && y < thisJewelClickableArea.bottom)
	{
		if (m_bIsSelected)
		{
			m_bIsSelected = false;
		}
		else
		{
			m_bIsSelected = true;
		}
	}

	return true;
}

bool AOJewel::HandleMouseClickUpAt(int x, int y)
{
	// in Bejeweled, a mouse-up event does nothing.
	return true;
}

bool AOJewel::HandleMouseHoverAt(int x, int y)
{
	// by now, x & y should be within this jewel... but just to be sure:

	Rect thisJewelClickableArea = GetClickableArea();
	if (x >= thisJewelClickableArea.left && x < thisJewelClickableArea.right &&
		y > thisJewelClickableArea.top && y < thisJewelClickableArea.bottom)
	{
		// hovering over this jewel - if we are not already in a hover state
		if (!m_bIsInHoverState)
		{
			// set hoverstate and reset hover animation
			m_bIsInHoverState = true;
			m_animationFrame = 1;
		}
	}

	return true;
}

void AOJewel::Remove()
{
	// This simply sets a flag - the AOJewel::Update() procedure handles the
	//  remove animation. When this animation is complete, the m_isDead bool
	//  is set to true. GameBoard::Update() will pick up on all m_isDead jewels
	//  and will respawn them.

	m_bIsBeingRemoved = true;
}

void AOJewel::SetDropTarget(int newYPosTarget)
{
	//if (newYPosTarget >= m_pixelDropTarget)
	{
		printf("Drop target dropping from %d to %d\n", m_pixelDropTarget, newYPosTarget);
		m_bIsDropping = true;
		m_pixelDropTarget = newYPosTarget;
	}
	//else
	{
		// trying to set a drop target above this jewel: use ResetDropTarget() instead!
		//SDL_assert(!"trying to set a drop target above this jewel: use ResetDropTarget() instead!");
	}
}

void AOJewel::ResetDropTarget()
{
	if (m_pixelDropTarget == m_pos.y)
		return;

	printf("Resetting drop target! From %d to %d\n", m_pixelDropTarget, m_pos.y);
	m_pixelDropTarget = m_pos.y;
}

void AOJewel::SetDropTargetRelative(int newYPosTarget)
{
	// This sets a drop target for the jewel - a destination height in pixels
	//  for the jewel to land at.
	assert(newYPosTarget > 0);
	printf("Drop target dropping from %d to %d\n", m_pixelDropTarget, m_pixelDropTarget + newYPosTarget);

	m_pixelDropTarget += newYPosTarget;
	m_bIsDropping = true;
}

#ifdef _DEBUG
void AOJewel::DebugDump()
{
	printf("Type %d \tPos %d %d \tSize %d ",
		m_jewelType, m_pos.x, m_pos.y, m_size.x);
	if (m_animationFrame != 0) printf("AnimFrame:%d ", m_animationFrame);
	printf("%s", m_bIsBeingRemoved ? "BeingRemoved " : "");
	printf("%s", m_bIsClickable ? "Clickable " : "");
	printf("%s", m_bIsDropping ? "IsDropping " : "");
	if (m_bIsDropping) printf("(to y:%d) ", m_pixelDropTarget);
	printf("%s", m_bIsSelected ? "IsSelected " : "");
	printf("%s", m_bIsVisible ? "IsVisible " : "");
	printf("DropTarget: %d \n", m_pixelDropTarget);

}
#endif //_DEBUG

bool AOJewel::SetPos(int x, int y)
{
	m_pos.x = x;
	m_pos.y = y;
	return true;
}

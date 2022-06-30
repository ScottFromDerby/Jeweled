#include "AOButton.h"
#include "AODialog.h"
#include "SDL.h"
#include "SDLRenderer.h"
#include "MouseCursor.h"
#include <assert.h>

////////////////////////////////////////////////////
// AOButton
//  An AOButton represents a clickable button on the
//  window. When clicked it has the ability to
//  trigger specific functionality. This is not yet
//  fully integrated with the Jeweled application,
//  but remains as it is a good example of the
//  extensibility of the code.
////////////////////////////////////////////////////

AOButton::AOButton(const VEC2& pos, const VEC2& size, const std::string& spriteRef, const std::string& spriteHoverRef, const std::string& spriteClickedRef, const std::string& spriteSelectedRef, BUTTON_TYPE buttonType)
	: ActiveObject(pos, size, spriteRef, spriteHoverRef, spriteClickedRef, spriteSelectedRef),
	m_buttonType(buttonType),
	m_hoveredOver(false),
	m_pushed(false)
{
}

int AOButton::Init()
{
	return 0;
}

bool AOButton::PerformClickedFunctionality()
{
	switch ( m_buttonType )
	{
	case BUTTON_TYPE_HINT:
		printf("Show Hint now! TODO!\n"); //TODO: Show Hint functionality
		break;
	case BUTTON_TYPE_NEWGAME:
		printf("New Game! TODO!\n"); //TODO: Make New Game Dialog
		break;
	case BUTTON_TYPE_NG_NORMAL:
		printf("Begin Normal Game! TODO!\n"); // TODO: Begin New Game:Normal
											  // (Reset) (+ Show modal Dialog)
		break;
	case BUTTON_TYPE_NG_TIMED:
		printf("Begin Timed Game! TODO!\n"); // TODO: Begin New Game:Timed
											 // (Reset) (+ Show modal Dialog)
		break;
	case BUTTON_TYPE_OPTIONS:
		printf("Options Dialog! Todo!\n"); //TODO: Show Options Dialog (modal)
		break;
	case BUTTON_TYPE_QUIT:
		printf("Quit Dialog! Todo!\n"); //TODO: Show Quit Dialog (modal)
		// must request to quit gracefully!
		break;
	default:
		assert(0); // unknown button type
		break;
	}
	return false;
}

int AOButton::Update()
{

	return 0;
}

int AOButton::Render()
{
	if (m_pushed)
	{
		if (m_spriteIDClicked.length() > 0)
		{
			SDLRenderer::Get()->DrawTexture(m_spriteIDClicked, m_pos.x, m_pos.y, m_size.x, m_size.y);
		}
	}
	else if (IsSelected())
	{
		if (m_spriteIDSelected.length() > 0)
		{
			SDLRenderer::Get()->DrawTexture(m_spriteIDSelected, m_pos.x, m_pos.y, m_size.x, m_size.y);
		}
	}
	else if (m_hoveredOver)
	{
		if (m_spriteIDHover.length() > 0)
		{
			SDLRenderer::Get()->DrawTexture(m_spriteIDHover, m_pos.x, m_pos.y, m_size.x, m_size.y);
		}
	}
	else if (IsVisible())
	{
		if (m_spriteIDBasic.length() > 0)
		{
			SDLRenderer::Get()->DrawTexture(m_spriteIDBasic, m_pos.x, m_pos.y, m_size.x, m_size.y);
		}
	}

	return 0;
}

bool AOButton::HandleMouseClickDownAt(int x, int y)
{
	// recheck if the cursor is OVER this button, rather than just
	//  checking m_hoveredOver.
	Rect ca = GetClickableArea();

	if ( x > ca.left && x < ca.right && y > ca.top && y < ca.bottom )
	{
		return PerformClickedFunctionality();
	}
	return true;
}

bool AOButton::HandleMouseHoverAt(int x, int y)
{
	m_hoveredOver = false;

	Rect ca = GetClickableArea();

	if ( x > ca.left && x < ca.right && y > ca.top && y < ca.bottom )
	{
		m_hoveredOver = true;
		MouseCursor::Get()->SetShowHand(true);
	}

	return true;
}

bool AOButton::HandleMouseClickUpAt(int x, int y)
{

	return true;
}
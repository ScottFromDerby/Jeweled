#include "AODialog.h"
#include "SDLRenderer.h"

////////////////////////////////////////////////////
// AODialog
//  An AODialog is a representation of a dialog 
//  which will be shown to the player. It is not
//  fully integrated with the Jeweled code, but like
//  AOButton, demonstrates a level of extensibility
//  in the Jeweled application.
////////////////////////////////////////////////////

AODialog::AODialog( VEC2 pos, VEC2 size, const char* spriteRef,
				   const char* spriteHoverRef, const char* spriteClickedRef,
				   const char* spriteSelectedRef ) : 
	ActiveObject( pos, size, spriteRef, spriteHoverRef, spriteClickedRef,
		spriteSelectedRef )
{
	
}

AODialog::~AODialog(void)
{
}

int AODialog::Init()
{
	return 0;
}

int AODialog::Update()
{
	return 0;
}

int AODialog::Render()
{
	return 0;
}

bool AODialog::HandleMouseClickDownAt(int x, int y)
{
	return false;
}

bool AODialog::HandleMouseClickUpAt(int x, int y)
{
	return false;
}

bool AODialog::HandleMouseHoverAt(int x, int y)
{
	return false;
}
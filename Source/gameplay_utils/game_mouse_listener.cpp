#include "game_mouse_listener.h"

#include <utils/debug.h>

#include "game_mouse.h"

//=============================================================================

GameMouseListener::GameMouseListener( int input_index, const std::string& input_name ) :
	mInputIndex( input_index ),
	mInputName( input_name )
{
	// add listener
	cassert( GameMouse::GetSingletonPtr() );
	GameMouse::GetSingletonPtr()->AddListener( this );

}

GameMouseListener::~GameMouseListener()
{
	// remove listener
	cassert( GameMouse::GetSingletonPtr() );
	GameMouse::GetSingletonPtr()->RemoveListener( this );
}

//=== event API =========================================================================

// return true if we've handled this and don't want the stuff below to get these messages
bool GameMouseListener::OnMouseMove( const types::vector2& screen_position ) 
{ 
	return false; 
}
	
// return true if we've handled this and don't want the stuff below to get these messages
bool GameMouseListener::OnMouseDown( const types::vector2& screen_position, int button ) 
{ 
	return false; 
}

// return true if we've handled this and don't want the stuff below to get these messages 
bool GameMouseListener::OnMouseUp( const types::vector2& screen_position, int button ) 
{
	return false; 
}
	
//=== poll API ==========================================================================

const types::vector2& GameMouseListener::GetMousePosition() const
{
	return mMousePosition;
}

bool GameMouseListener::IsMouseButtonDown( int button ) const
{
	cassert( MouseDebugMakeSureButtonInCorrectFormat( button ) );
	return ( mMouseButtonsDown & button ) ? true : false;
}

bool GameMouseListener::IsMouseButtonJustDown( int button ) const
{
	cassert( MouseDebugMakeSureButtonInCorrectFormat( button ) );
	return ( mMouseButtonsJustDown & button ) ? true : false;
}

bool GameMouseListener::IsMouseButtonJustUp( int button ) const
{
	cassert( MouseDebugMakeSureButtonInCorrectFormat( button ) );
	return ( mMouseButtonsJustUp & button ) ? true : false;
}

//=== privates ==========================================================================

bool GameMouseListener::OnMouseMove_Private(const types::vector2& screen_position)
{
	mMousePosition = screen_position;
	return OnMouseMove( screen_position );
}

bool GameMouseListener::OnMouseDown_Private(const types::vector2& screen_position, int button)
{
	cassert(MouseDebugMakeSureButtonInCorrectFormat(button));

	mMouseButtonsDown |= button;
	mMouseButtonsJustDown |= button;
	return OnMouseDown( screen_position, button );
}

bool GameMouseListener::OnMouseUp_Private(const types::vector2& screen_position, int button)
{
	cassert(MouseDebugMakeSureButtonInCorrectFormat(button));

	mMouseButtonsDown = mMouseButtonsDown & (~button);
	mMouseButtonsJustUp |= button;
	return OnMouseUp( screen_position, button );
}

void GameMouseListener::OnFrameEnd()
{
	mMouseButtonsJustDown = GameMouseButton::NONE;
	mMouseButtonsJustUp = GameMouseButton::NONE;
}

bool GameMouseListener::MouseDebugMakeSureButtonInCorrectFormat(int button) const
{
	if (button == GameMouseButton::NONE) return true;
	if (button == GameMouseButton::LEFT) return true;
	if (button == GameMouseButton::RIGHT) return true;
	if (button == GameMouseButton::MIDDLE) return true;
	if (button == GameMouseButton::WHEEL_UP) return true;
	if (button == GameMouseButton::WHEEL_DOWN) return true;

	return false;
}
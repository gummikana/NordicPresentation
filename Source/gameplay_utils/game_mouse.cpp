#include "game_mouse.h"

#include <algorithm>
#include <utils/vector_utils/vector_utils.h>
#include <utils/debug.h>

#include <poro/iplatform.h>
#include <poro/mouse.h>

#include "game_mouse_listener.h"


//-----------------------------------------------------------------------------
namespace 
{

bool compare_GameMouseListener( GameMouseListener* lhs, GameMouseListener* rhs)
{
	return lhs->GetInputIndex() < rhs->GetInputIndex();
}

} // end of anonymous namespace

//-----------------------------------------------------------------------------

int GameMouse::ConvertButton( int poro_button )
{
	if( poro_button <= poro::Mouse::MOUSE_BUTTON_RIGHT ) 
		return poro_button;
	
	switch( poro_button )
	{
	case poro::Mouse::MOUSE_BUTTON_MIDDLE:
		return GameMouseButton::MIDDLE;
		break;
	case poro::Mouse::MOUSE_BUTTON_WHEEL_UP:
		return GameMouseButton::WHEEL_UP;
		break;
	case poro::Mouse::MOUSE_BUTTON_WHEEL_DOWN:
		return GameMouseButton::WHEEL_DOWN;
		break;
	default:
		return GameMouseButton::NONE;
		break;
	}
}

//-----------------------------------------------------------------------------

GameMouse::GameMouse()
{
}

GameMouse::~GameMouse()
{
}

//-----------------------------------------------------------------------------

void GameMouse::Init()
{
	// add listener to Poro()
	Poro()->GetMouse()->AddMouseListener( this );
}

//-----------------------------------------------------------------------------

void GameMouse::AddListener( GameMouseListener* listener )
{
	if( ceng::VectorAddUnique( mListeners, listener ) ) 
	{
		std::sort( mListeners.begin(), mListeners.end(), compare_GameMouseListener );
	}
}

void GameMouse::RemoveListener( GameMouseListener* listener )
{
	if( ceng::VectorRemove( mListeners, listener ) )
	{
		std::sort( mListeners.begin(), mListeners.end(), compare_GameMouseListener );
	}
}

//-----------------------------------------------------------------------------

const std::vector< GameMouseListener* >& GameMouse::GetListeners() const
{
	return mListeners;
}

//-----------------------------------------------------------------------------

void GameMouse::OnFrameEnd()
{
	for( std::size_t i = 0; i < mListeners.size(); ++i )
	{
		GameMouseListener* listener = mListeners[ i ];
		cassert( listener );

		// end the execution if the event is handeled
		listener->OnFrameEnd();
	}
}

//-----------------------------------------------------------------------------

void GameMouse::MouseMove( const poro::types::Vec2& pos ) 
{ 
	const types::vector2 mouse_pos( pos );
	for( std::size_t i = 0; i < mListeners.size(); ++i )
	{
		GameMouseListener* listener = mListeners[ i ];
		cassert( listener );

		// end the execution if the event is handeled
		if( listener->OnMouseMove_Private( mouse_pos ) )
			return;
	}
}

void GameMouse::MouseButtonDown( const poro::types::Vec2& pos, int button ) 
{ 
	const types::vector2 mouse_pos( pos );
	int game_button = ConvertButton( button );
	GameMouseListener* listener = NULL;
	for( std::size_t i = 0; i < mListeners.size(); ++i )
	{
		listener = mListeners[ i ];
		cassert( listener );

		// end the execution if the event is handeled
		if( listener->OnMouseDown_Private( mouse_pos, game_button ) )
			return;
	}
}

void GameMouse::MouseButtonUp( const poro::types::Vec2& pos, int button ) 
{ 
	const types::vector2 mouse_pos( pos );
	int game_button = ConvertButton( button );
	GameMouseListener* listener = NULL;
	for( std::size_t i = 0; i < mListeners.size(); ++i )
	{
		listener = mListeners[ i ];
		cassert( listener );

		// end the execution if the event is handeled
		if( listener->OnMouseUp_Private( mouse_pos, game_button ) )
			return;
	}
}

//-----------------------------------------------------------------------------

#ifndef INC_GAME_MOUSE_LISTENER_H
#define INC_GAME_MOUSE_LISTENER_H

#include <string>
#include <utils/math/cvector2.h>

namespace GameInputIndex 
{
	// the order of game inputs are handled in here
	enum {
		Debug_Inspector = 0,
		Debug_Default = 50,
		LuaUI_Default = 100,
		RtsSelection = 200,
		GodSpells = 300,
		Default_PlayArea = 400
	};
}

//-----------------------------------------------------------------------------

// IMPORTANT! 
/* When using this class, remember to also to call the functions

	GameMouse::GetSingletonPtr()->Init();

 and in the update
	GameMouse::GetSingletonPtr()->OnFrameEnd();

*/

class GameMouseListener
{
	friend class GameMouse;

public:
	
	// Auto registers itself in the constructor, and deregisters in the deconstructor
	GameMouseListener( int input_index, const std::string& input_name );
	virtual ~GameMouseListener();
	
	// inputs and names
	virtual int			GetInputIndex() const { return mInputIndex; }
	virtual std::string GetInputName() const { return mInputName; }

	// === event API ====================================================================

	// return true if we've handled this and don't want the stuff below to get these messages
	virtual bool OnMouseMove( const types::vector2& screen_position );
	
	// return true if we've handled this and don't want the stuff below to get these messages
	// note the button we get is GameMouseButtonType button, not poro::Mouse::MOUSE_BUTTON_*
	// the main difference is from MIDDLE onworth, the buttons are binary compatible instead of 
	// being in a sequence
	virtual bool OnMouseDown( const types::vector2& screen_position, int button );

	// return true if we've handled this and don't want the stuff below to get these messages
	// NOTE! You should think couple of times if you want to capture mouse up results at any point
	// since capturing them can lead to some mouse buttons being 'down' while not really being down
	virtual bool OnMouseUp( const types::vector2& screen_position, int button );
	
	//=== poll API =====================================================================

	// getters to current situations
	virtual const types::vector2& GetMousePosition() const;
	
	// Note! parameter is in GameMouseButtonType format not poro::Mouse::MouseType format
	virtual bool IsMouseButtonDown( int button ) const;
	
	// Note! parameter is in GameMouseButtonType format not poro::Mouse::MouseType format
	virtual bool IsMouseButtonJustDown( int button ) const;
	
	// Note! parameter is in GameMouseButtonType format not poro::Mouse::MouseType format
	virtual bool IsMouseButtonJustUp(int button) const;

private:

	// return true if we've handled this and don't want the stuff below to get these messages
	bool OnMouseMove_Private(const types::vector2& screen_position);

	// return true if we've handled this and don't want the stuff below to get these messages
	// note the button we get is GameMouseButtonType button, not poro::Mouse::MOUSE_BUTTON_*
	// the main difference is from MIDDLE onworth, the buttons are binary compatible instead of 
	// being in a sequence
	bool OnMouseDown_Private(const types::vector2& screen_position, int button);

	// return true if we've handled this and don't want the stuff below to get these messages
	// NOTE! You should think couple of times if you want to capture mouse up results at any point
	// since capturing them can lead to some mouse buttons being 'down' while not really being down
	bool OnMouseUp_Private(const types::vector2& screen_position, int button);

	// this nulls the just downs
	void OnFrameEnd();

	bool MouseDebugMakeSureButtonInCorrectFormat( int button ) const;


	types::vector2 mMousePosition;

	int				mInputIndex;
	std::string		mInputName;

	// combined button states
	int mMouseButtonsDown;
	int mMouseButtonsJustDown;
	int mMouseButtonsJustUp;
};

//-----------------------------------------------------------------------------

#endif

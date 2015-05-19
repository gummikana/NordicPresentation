#ifndef INC_GAME_MOUSE_H
#define INC_GAME_MOUSE_H

#include <vector>
#include <poro/imouse_listener.h>
#include <utils/singleton/csingleton.h>

class GameMouseListener;

// GameMouse is a singleton class for Wizard Physics to be used as the Mouse Interface instead of Poro()->GetMouse()
// reason for this is that Poro() Mouse doesn't really handle focus in any way and since Wizard Physics has 
// quite a bit UI cases and elements, we want a bit more elegant way of handling Mouse cases
// 
// For quick debug hacks it's fine to use Poro()->GetMouse()->IsButtonDown() etc...
// but if what you're doing needs not to happen when the mouse is on clicking on an UI element, then
// inherit 
// GameMouseListener
// and setup your input index so that's it is in the correct position for inputs to triggle down
// and use that GameMouseListeners IsButtonDown() functions
// also if you need to 'capture' mouse inputs make sure to implement your own OnMouseDown() 
// and return true when you've 'captured' the mouse event  so it doesn't get triggled down to
// the next element

namespace GameMouseButton
{
	enum GameMouseButtonType
	{
		NONE = 0,
		LEFT = 1,
		RIGHT = 2,
		MIDDLE = 4,
		WHEEL_UP = 8,
		WHEEL_DOWN = 16,
	};
}

//-----------------------------------------------------------------------------

class GameMouse : public ceng::CStaticSingleton< GameMouse >, public poro::IMouseListener
{
public:
	~GameMouse();

	void Init();

	// void Update()
	// this is the last thing we want to call in our IDefaultApplication::Update( float ) function
	void OnFrameEnd();

	//-------------------------------------------------------------------------
	// listeners
	//.........................................................................

	void AddListener( GameMouseListener* listener );
	void RemoveListener( GameMouseListener* listener );
	
	const std::vector< GameMouseListener* >& GetListeners() const;
	
	//-------------------------------------------------------------------------
	// getters to the mouse state
	
	// returns the current mouse position
	/*
	const types::vector2&	GetMousePosition() const;
	bool					IsMouseButtonDown( int button ) const;
	bool					IsMouseButtonJustDown( int button ) const;
	*/

	// returns GameMouseButtonType button
	static int ConvertButton( int poro_button );

	//-------------------------------------------------------------------------
	// poro listeners
	//.........................................................................
protected:
	virtual void MouseMove( const poro::types::Vec2& pos );
	virtual void MouseButtonDown( const poro::types::Vec2& pos, int button );
	virtual void MouseButtonUp( const poro::types::Vec2& pos, int button );


	//-------------------------------------------------------------------------

private:
	GameMouse();
	friend class ceng::CStaticSingleton< GameMouse >;

	std::vector< GameMouseListener* > mListeners;
};

//-----------------------------------------------------------------------------

#endif

#ifndef INC_SIMPLE_UI_H
#define INC_SIMPLE_UI_H

#include <vector>
#include <poro/poro.h>
#include <game_utils/actionscript/actionscript.h>

#include "../gameplay_utils/game_mouse_listener.h"

namespace as { class Sprite; }

//-----------------------------------------------------------------------------

class SimpleUIListener
{
public:
	virtual  ~SimpleUIListener() { }

	virtual void OnClick( as::Sprite* sprite, const std::string& name_of_sprite ) 
	{
	}

	virtual void OnSlideTo( as::Sprite* sprite, float value ) 
	{
	}

	virtual void OnSlideEnd( as::Sprite* sprite, float value )
	{
	}

	virtual void OnCheckboxChange( as::Sprite* sprite, bool value )
	{
	}

	virtual void OnTextboxChange( as::Sprite* sprite, const std::string& value )
	{
	}

	virtual void OnTextboxFocusChange( as::Sprite* sprite, bool has_focus ) 
	{
	}
};

//-----------------------------------------------------------------------------

// change the mouse base if needed
class SimpleBase : public as::Sprite, public GameMouseListener
{
public:
	SimpleBase();
	virtual ~SimpleBase();

	virtual types::vector2		TransformMousePos( const types::vector2& mouse_pos );

	void MouseButtonDown(const poro::types::vec2& pos, int button);
	void MouseButtonUp(const poro::types::vec2& pos, int button);
	void MouseMove(const poro::types::vec2& pos );

	virtual bool OnMouseMove( const types::vector2& screen_position );
	virtual bool OnMouseDown( const types::vector2& screen_position, int button );
	virtual bool OnMouseUp( const types::vector2& screen_position, int button );

	void* mUserData;
	SimpleUIListener* mListener;
};

class SimpleBaseText : public as::TextSprite, public GameMouseListener 
{
public:
	SimpleBaseText();
	virtual ~SimpleBaseText();

	virtual types::vector2		TransformMousePos( const types::vector2& mouse_pos );

	void MouseButtonDown(const poro::types::vec2& pos, int button);
	void MouseButtonUp(const poro::types::vec2& pos, int button);
	void MouseMove(const poro::types::vec2& pos );

	virtual bool OnMouseMove( const types::vector2& screen_position );
	virtual bool OnMouseDown( const types::vector2& screen_position, int button );
	virtual bool OnMouseUp( const types::vector2& screen_position, int button );

	void* mUserData;
	SimpleUIListener* mListener;
};

//-----------------------------------------------------------------------------

class SimpleButton : public SimpleBase
{
public:
	SimpleButton( SimpleUIListener* listener = NULL );
	virtual ~SimpleButton();

	virtual bool				OnMouseDown( const types::vector2& screen_position, int button );
};

//-----------------------------------------------------------------------------

class SimpleSlider : public SimpleBase
{
public:
	SimpleSlider( SimpleUIListener* listener = NULL );
	~SimpleSlider();

	// mouse listeners
	virtual bool OnMouseMove( const types::vector2& screen_position );
	virtual bool OnMouseDown( const types::vector2& screen_position, int button );
	virtual bool OnMouseUp( const types::vector2& screen_position, int button );

	
	float RelativeDistance( const types::vector2& start, const types::vector2& end, const types::vector2& current );

	void SetSliderValue( float value, bool call_callbacks = false );
	float GetSliderValue() const;
	
	bool mMouseDown;
	types::vector2 mMouseDownOffset;
	types::vector2 mMouseDownPos;
	
	types::vector2 mStartPos;
	types::vector2 mEndPos;
	float mValue;
};

//-----------------------------------------------------------------------------

class SimpleCheckbox : public SimpleBase
{
public:
	SimpleCheckbox( SimpleUIListener* listener = NULL );
	~SimpleCheckbox();

	virtual bool OnMouseDown( const types::vector2& screen_position, int button );

	void SetValue( bool value );

	bool mValue;
	std::string mCheckboxSpriteName;
};

//-----------------------------------------------------------------------------

class SimpleTextbox : public SimpleBaseText, public poro::IKeyboardListener
{
public:
	SimpleTextbox( SimpleUIListener* listener = NULL );
	~SimpleTextbox();

	virtual bool OnMouseDown( const types::vector2& screen_position, int button );


	void SetValue( const std::string& value );

	std::string GetValue() const;
	bool HasFocus() const;

	virtual void OnLoseFocus();
	virtual void OnEnter();

	virtual void OnKeyDown( int key, poro::types::charset unicode );
	virtual void OnKeyUp( int key, poro::types::charset unicode );

	virtual void SetFocus( bool value );

	void SetAsFileDialog( bool is_file_dialog, const std::string& path = "data\\" );

	virtual void		SetText( const std::string& text );
	virtual std::string	GetText() const;


	bool mFocus;
	std::string mValue;
	bool mClear;
	bool mIsFileDialog;
	std::string mFileDialogPath;
	std::string mTextboxName;
};

//-----------------------------------------------------------------------------

class SimpleDropDown : public SimpleButton
{
public:
	SimpleDropDown( SimpleUIListener* listener = NULL );
	~SimpleDropDown();

	virtual bool OnMouseDown( const types::vector2& screen_position, int button );

	void ShowDropDowns();
	void HideDropDowns();

	void AddDropDown( as::Sprite* drop_down );

protected:
	std::vector< as::Sprite* > mDropDowns;	
	bool mDropDownsVisible;
};

//-----------------------------------------------------------------------------

#endif

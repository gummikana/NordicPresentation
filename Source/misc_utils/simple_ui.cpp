#include "simple_ui.h"
#include "file_dialog.h"

#include <stdio.h>  /* defines FILENAME_MAX */
#include <poro/platform_defs.h>

#include "../gameplay_utils/game_mouse.h"

#ifdef PORO_PLAT_WINDOWS
    #include <direct.h>
    #define GetCurrentDir _getcwd
#else
    #include <unistd.h>
    #define GetCurrentDir getcwd
 #endif


namespace {
	std::string AsRelativePath( std::string filepath )
	{
		char current_path[FILENAME_MAX];

		if (!GetCurrentDir(current_path, sizeof(current_path)))
			return filepath;
		
		// std::cout << "Filepath: " << filepath << ", current_path: " << current_path << std::endl;
		
		std::size_t substr_pos = 0;
		for( std::size_t i = 0; i < filepath.size() && i < FILENAME_MAX; ++i )
		{
			if( tolower( filepath[ i ] ) != tolower( current_path[ i ] ) )
				break;

			substr_pos++;
		}

		filepath = filepath.substr( substr_pos );
		if( filepath.empty() == false && filepath[ 0 ] == '\\' ) 
			filepath = filepath.substr( 1 );


		filepath = ceng::StringReplace( "\\", "/", filepath );
		return filepath;
	}
}

//-----------------------------------------------------------------------------

SimpleBase::SimpleBase() :
	as::Sprite(),
	GameMouseListener( GameInputIndex::Debug_Default, "DebugLayer" ),
	mUserData( NULL ),
	mListener( NULL )
{
	// if( Poro() && Poro()->GetMouse() ) 
	//	Poro()->GetMouse()->AddMouseListener( this );
}

SimpleBase::~SimpleBase() 
{
	//if( Poro() && Poro()->GetMouse() ) 
	//	Poro()->GetMouse()->RemoveMouseListener( this );
}

types::vector2 SimpleBase::TransformMousePos( const types::vector2& mouse_pos )
{
	types::vector2 pos = TransformWithAllParents( mouse_pos );
	return pos;

	types::vector2 result( 0, 0 );
	std::vector< const DisplayObjectContainer* > parents;
	getParentTree( parents );

	types::xform xform;
	for( int i = (int)parents.size() - 1; i > 0; --i )
	{
		cassert( parents[ i ] );
		const Sprite* parent = dynamic_cast< const Sprite* >( parents[ i ] );
		if( parent )
			xform = ceng::math::Mul( xform, parent->GetXForm() );
	}

	// xform = ceng::math::Mul( xform, this->GetXForm() );

	return ceng::math::MulT( xform, mouse_pos );
}

void SimpleBase::MouseButtonDown(const poro::types::vec2& pos, int button) {
	int nbutton = GameMouse::ConvertButton( button );
	OnMouseDown( types::vector2( pos ), nbutton );
}

void SimpleBase::MouseButtonUp(const poro::types::vec2& pos, int button) {
	int nbutton = GameMouse::ConvertButton( button );
	OnMouseUp( types::vector2( pos ), nbutton );
}

void SimpleBase::MouseMove(const poro::types::vec2& pos ) {
	OnMouseMove( types::vector2( pos ) );
}


bool SimpleBase::OnMouseMove( const types::vector2& screen_position ) {
	return false;
}

bool SimpleBase::OnMouseDown( const types::vector2& screen_position, int button ) {
	return false;
}

bool SimpleBase::OnMouseUp( const types::vector2& screen_position, int button ) {
	return false;
}
//-----------------------------------------------------------------------------

SimpleBaseText::SimpleBaseText() :
	as::TextSprite(),
	GameMouseListener( GameInputIndex::Debug_Default, "DebugLayer" ),
	mUserData( NULL ),
	mListener( NULL )
{
	// if( Poro() && Poro()->GetMouse() ) 
	//	Poro()->GetMouse()->AddMouseListener( this );
}

SimpleBaseText::~SimpleBaseText() 
{
	//if( Poro() && Poro()->GetMouse() ) 
	//	Poro()->GetMouse()->RemoveMouseListener( this );
}

types::vector2 SimpleBaseText::TransformMousePos( const types::vector2& mouse_pos )
{
	types::vector2 result( 0, 0 );
	std::vector< const DisplayObjectContainer* > parents;
	getParentTree( parents );

	types::xform xform;
	for( int i = (int)parents.size() - 1; i > 0; --i )
	{
		cassert( parents[ i ] );
		const Sprite* parent = dynamic_cast< const Sprite* >( parents[ i ] );
		if( parent )
			xform = ceng::math::Mul( xform, parent->GetXForm() );
	}

	// xform = ceng::math::Mul( xform, this->GetXForm() );

	return ceng::math::MulT( xform, mouse_pos );
}

void SimpleBaseText::MouseButtonDown(const poro::types::vec2& pos, int button) {
	int nbutton = GameMouse::ConvertButton( button );
	OnMouseDown( types::vector2( pos ), nbutton );
}

void SimpleBaseText::MouseButtonUp(const poro::types::vec2& pos, int button) {
	int nbutton = GameMouse::ConvertButton( button );
	OnMouseUp( types::vector2( pos ), nbutton );
}

void SimpleBaseText::MouseMove(const poro::types::vec2& pos ) {
	OnMouseMove( types::vector2( pos ) );
}


bool SimpleBaseText::OnMouseMove( const types::vector2& screen_position ) {
	return false;
}

bool SimpleBaseText::OnMouseDown( const types::vector2& screen_position, int button ) {
	return false;
}

bool SimpleBaseText::OnMouseUp( const types::vector2& screen_position, int button ) {
	return false;
}

//-----------------------------------------------------------------------------

SimpleButton::SimpleButton( SimpleUIListener* listener ) : SimpleBase() 
{
	mUserData = NULL;
	mListener = listener;
}

SimpleButton::~SimpleButton()
{
}

bool SimpleButton::OnMouseDown( const types::vector2& pos, int button )
{
	if( mListener == NULL ) return false;
	if( GetVisibility() == false ) return false;

	if( button == 1 ) 
	{
		const types::vector2 relative_mouse_pos = TransformMousePos( pos );
		std::vector< Sprite* > sprites;
		sprites = FindSpritesAtPoint( relative_mouse_pos );

		if( sprites.empty() ) return false;
		std::string name = "";

		for( int i = sprites.size() - 1; i >= 0; --i )
		{
			if( name.empty() && sprites[ i ] && sprites[ i ]->GetName().empty() == false )
				name = sprites[ i ]->GetName();
		}

		mListener->OnClick( this, name );
		return true;
	}

	return false;
}

//-----------------------------------------------------------------------------

SimpleSlider::SimpleSlider( SimpleUIListener* listener  ) : SimpleBase() 
{
	mListener = listener;
	mMouseDown = false;
}

SimpleSlider::~SimpleSlider()
{
}

bool SimpleSlider::OnMouseDown( const types::vector2& pos, int button)
{
	if( GetVisibility() == false ) return false;

	if( button == 1 ) 
	{
		const types::vector2 relative_mouse_pos = TransformMousePos( pos );

		std::vector< Sprite* > sprites;
		sprites = /*((Sprite*)(getParent()))->*/FindSpritesAtPoint( relative_mouse_pos );

		if( sprites.empty() ) return false;
		
		/*std::string name = "";
		

		for( int i = sprites.size() - 1; i >= 0; --i )
		{
			if( name.empty() && sprites[ i ] && sprites[ i ]->GetName().empty() == false )
				name = sprites[ i ]->GetName();
		}*/

		mMouseDown = true;
		mMouseDownOffset = this->GetPos() - relative_mouse_pos;
		mMouseDownPos = relative_mouse_pos;
		return true;
	}
	return false;
}

bool SimpleSlider::OnMouseUp(const types::vector2& pos, int button)
{
	if( mMouseDown && button == 1 ) 
	{
		OnMouseMove( pos );
		mMouseDown = false;
		if( mListener ) mListener->OnSlideEnd( this, mValue );
		return true;
	}

	return false;
}

bool SimpleSlider::OnMouseMove(const types::vector2& pos )
{
	if( mMouseDown ) 
	{
		types::vector2 relative_mouse_pos = TransformMousePos( pos );
		// std::cout << "MouseMove pos: " << relative_mouse_pos.x << ", " << relative_mouse_pos.y << std::endl;
		if( Poro()->GetKeyboard()->IsShiftDown() )
			relative_mouse_pos = ( ( relative_mouse_pos - mMouseDownPos ) * 0.01f ) + mMouseDownPos;

		const types::vector2 target = relative_mouse_pos + mMouseDownOffset;
		
		const types::vector2 placement = ceng::math::ClosestPointOnLineSegment( mStartPos, mEndPos, target );

		MoveTo( placement );

		const float new_value = RelativeDistance( mStartPos, mEndPos, placement );
		if( new_value != mValue )
		{
			mValue = new_value;
			if( mListener ) mListener->OnSlideTo( this, mValue );
		}
		return true;
	}

	return false;
}

float SimpleSlider::RelativeDistance( const types::vector2& start, const types::vector2& end, const types::vector2& current )
{
	if( end == start ) return 1.f;

	const float distance_to_start = ( current - start ).Length();
	const float distance_to_end = ( end - current ).Length();
	const float total_distance = distance_to_start + distance_to_end;	
	
	return (float)( distance_to_start / (total_distance) );
}

void SimpleSlider::SetSliderValue( float value, bool call_callbacks )
{
	if( mMouseDown == false || call_callbacks /*&& value != mValue*/ )
	{
		if( value != mValue )
		{
			mValue = value;
			if( call_callbacks && mListener ) mListener->OnSlideTo( this, mValue );
		}

		const types::vector2 position = ceng::math::Lerp( mStartPos, mEndPos, ceng::math::Clamp( mValue, 0.f, 1.f ) );
		MoveTo( position );
	}
}

float SimpleSlider::GetSliderValue() const { return mValue; }

//-----------------------------------------------------------------------------

SimpleCheckbox::SimpleCheckbox( SimpleUIListener* listener ) : 
	SimpleBase()
{
	mListener = listener;
	mValue = false;
	mCheckboxSpriteName = "checkbox_overlay";
}

SimpleCheckbox::~SimpleCheckbox()
{
}

bool SimpleCheckbox::OnMouseDown(const types::vector2& pos, int button)
{
	if( GetVisibility() == false ) return false;

	if( button == 1 ) 
	{
		const types::vector2 relative_mouse_pos = TransformMousePos( types::vector2( pos ) );
		std::vector< Sprite* > sprites = FindSpritesAtPoint( relative_mouse_pos );

		if( sprites.empty() ) return false;
		
		SetValue( !mValue );
		return true;
	}

	return false;
}

void SimpleCheckbox::SetValue( bool value ) 
{
	if( value != mValue ) 
	{
		mValue = value;

		as::Sprite* overlay = GetChildByName( mCheckboxSpriteName );

		if( overlay && mValue == true )			overlay->SetVisibility( true );
		else if( overlay && mValue == false )	overlay->SetVisibility( false );

		if( mListener ) 
			mListener->OnCheckboxChange( this, mValue );
	}
}


//-----------------------------------------------------------------------------

SimpleTextbox::SimpleTextbox( SimpleUIListener* listener ) : 
	SimpleBaseText()
{
	if( Poro() && Poro()->GetKeyboard() ) 
		Poro()->GetKeyboard()->AddKeyboardListener( this );

	mIsFileDialog = false;
	mListener = listener;
	mValue = "";
	mTextboxName = "text_element";
	SetFocus( false );
}

SimpleTextbox::~SimpleTextbox()
{
	if( Poro() && Poro()->GetKeyboard() ) 
		Poro()->GetKeyboard()->RemoveKeyboardListener( this );
}

bool SimpleTextbox::OnMouseDown(const types::vector2& pos, int button)
{
	if( GetVisibility() == false ) return false;

	if( button == 1 ) 
	{
		bool focus_before = mFocus;
		SetFocus( false );

		const types::vector2 relative_mouse_pos = TransformMousePos( types::vector2( pos ) );
		std::vector< Sprite* > sprites = FindSpritesAtPoint( relative_mouse_pos );

		if( sprites.empty() ) 
		{
			if( focus_before ) 
				OnLoseFocus();

			return false;
		}
		
		SetFocus( true );
		mClear = true;

		// file dialog...
		if( mIsFileDialog ) 
		{
			mClear = false;
			std::string filename = LoadFileDialog( mFileDialogPath );
			filename = AsRelativePath( filename );
			
			SetValue( filename );
			this->OnEnter();

			SetFocus( false );
			return true;
		}

		return true;
	}
	return false;
}

void SimpleTextbox::SetValue( const std::string& value ) 
{
	if( value != mValue ) 
	{
		mValue = value;

		as::Sprite* overlay = GetChildByName( mTextboxName );
		as::TextSprite* overlay_text = dynamic_cast< as::TextSprite* >( overlay );

		if( overlay_text )
			overlay_text->SetText( value );

		/*if( mListener ) 
			mListener->OnCheckboxChange( this, mValue );*/
	}
}

std::string SimpleTextbox::GetValue() const { return mValue; }
bool SimpleTextbox::HasFocus() const { return mFocus; }

void SimpleTextbox::OnLoseFocus()
{
	if( mListener ) { mListener->OnTextboxChange( this, mValue ); }
	SetFocus( false );
}

void SimpleTextbox::OnEnter()
{
	if( mListener ) { mListener->OnTextboxChange( this, mValue ); }
}

void SimpleTextbox::OnKeyDown( int key, poro::types::charset unicode ) 
{
	if( mFocus == false ) return;
	if( mClear ) { SetValue( "" ); mClear = false; }

	// backspace
	if( key == 8 && mValue.size() > 0 ) SetValue( mValue.substr( 0, mValue.size() - 1 ) );
	// enter
	else if( key == 13 ) OnEnter();
	// space
	else if( key >= 32 && key <= 126 ) SetValue( mValue + (char)unicode );
}

void SimpleTextbox::OnKeyUp( int key, poro::types::charset unicode ) 
{
}

void SimpleTextbox::SetFocus( bool value ) 
{
	if( mFocus != value ) {
		mFocus = value;
		if( mListener ) mListener->OnTextboxFocusChange( this, mFocus );
	}
}

void SimpleTextbox::SetAsFileDialog( bool is_file_dialog,  const std::string& path  )
{
	mIsFileDialog = is_file_dialog;
	mFileDialogPath = path;
}


void SimpleTextbox::SetText( const std::string& text ) {
	mValue = text;
	as::TextSprite* t = dynamic_cast< as::TextSprite* >( GetChildByName( mTextboxName ) );
	if( t ) t->SetText( text );

	// as::Sprite::SetText( text );
	// as::TextSprite::SetText( text );
}

std::string	SimpleTextbox::GetText() const {
	return mValue;
}

//-----------------------------------------------------------------------------

SimpleDropDown::SimpleDropDown( SimpleUIListener* listener ) : 
	SimpleButton( listener )
{
	mDropDownsVisible = false;
}

SimpleDropDown::~SimpleDropDown()
{
	
}

bool SimpleDropDown::OnMouseDown( const types::vector2& pos, int button)
{
	if( mListener == NULL ) return false;
	if( GetVisibility() == false ) return false;

	if( button == 1 ) 
	{
		const types::vector2 relative_mouse_pos = TransformMousePos( types::vector2( pos ) );
		std::vector< Sprite* > sprites;
		sprites = FindSpritesAtPoint( relative_mouse_pos );

		if( mDropDownsVisible == false ) 
		{
			if( sprites.empty() ) return false;
			
			// we hit something
			ShowDropDowns();
			return true;
		}
		else
		{
			
			// mis click
			if( sprites.empty() )
			{
				HideDropDowns();
				return false;
			}
			std::string name = "";
	

			for( int i = sprites.size() - 1; i >= 0; --i )
			{
				if( name.empty() && sprites[ i ] && sprites[ i ]->GetName().empty() == false )
					name = sprites[ i ]->GetName();
			}

			HideDropDowns();
			mListener->OnClick( this, name );
			return true;
		}
	}

	return false;
}

void SimpleDropDown::ShowDropDowns()
{
	mDropDownsVisible = true;
	for( std::size_t i = 0; i < mDropDowns.size(); ++i  )
	{
		cassert( mDropDowns[ i ] );
		mDropDowns[ i ]->SetVisibility( true );
	}
}

void SimpleDropDown::HideDropDowns()
{
	mDropDownsVisible = false;
	for( std::size_t i = 0; i < mDropDowns.size(); ++i  )
	{
		cassert( mDropDowns[ i ] );
		mDropDowns[ i ]->SetVisibility( false );
	}
}

void SimpleDropDown::AddDropDown( as::Sprite* drop_down ) 
{
	cassert( drop_down );
	addChild( drop_down );
	mDropDowns.push_back( drop_down );
	drop_down->SetVisibility( mDropDownsVisible );
}
//-----------------------------------------------------------------------------

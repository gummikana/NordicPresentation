#include "debug_layer.h"

#include <SDL.h>
#include <utils/math/point_inside.h>
#include <game_utils/tween/tween_utils.h>
#include <game_utils/drawlines/drawlines.h>

#include "screenshotter.h"
#include "simple_profiler_viewer.h"
#include "file_dialog.h"

#include "../gameplay_utils/game_mouse.h"

#define DEBUG_LAYER_DONT_USE_COMPONENTS
#ifndef DEBUG_LAYER_DONT_USE_COMPONENTS

#include "../component_framework/ComponentMacro.h"
#include "../component_framework/Component.h"
#include "../component_framework/ComponentFactory.h"
#include "../component_framework/EventManager.h"
#include "../component_framework/EntityManager.h"
#include "../component_framework/ComponentMapper.h"
#include "../component_framework/EntitySerializer.h"

#endif


//-----------------------------------------------------------------------------

class ProfilerBars : public as::Sprite
{
public:
	ProfilerBars() :
		mPos( 0 )
	{
		if( mCPUTimeSteps.size() == 0 )
			mCPUTimeSteps.resize( 120 * 2 );
	}

	virtual void Update( float dt )
	{
		const float frame_should_last = 1.f / Poro()->GetFrameRate();
		float t = (float)Poro()->GetLastFrameExecutionTime() / frame_should_last;
		mPos = Poro()->GetFrameNum() % (int)mCPUTimeSteps.size();


		mCPUTimeSteps[ mPos ] = t;
	}

	virtual bool Draw( poro::IGraphics* graphics, types::camera* camera, as::Transform& transform )
	{
		as::Sprite::Draw( graphics, camera, transform );

		const float height = 64.f;
		float pos_x = (float)mCPUTimeSteps.size();

		for( int i = 0; i < (int)mCPUTimeSteps.size(); ++i ) 
		{
			int p = mPos - i;
			if( p < 0 ) p += (int)mCPUTimeSteps.size();
			pos_x -= 1.f;

			float t = mCPUTimeSteps[ p ];

			types::vector2 p0( pos_x, height );
			types::vector2 p1( pos_x, (1.f - t) * height );

			transform.PushXForm( this->mXForm, this->mColor );

			p0 = ceng::math::Mul( transform.GetXForm(), p0 - mCenterOffset );
			p1 = ceng::math::Mul( transform.GetXForm(), p1 - mCenterOffset );

			transform.PopXForm();

			DrawLine( graphics, p0, p1, FindColor( t ) );
		}

		
		return true;
	}

	poro::types::fcolor FindColor( float t ) 
	{
		return poro::GetFColor( t, 1.f - t, 0.f, 1.f );
	}

	static std::vector< float > mCPUTimeSteps;
	int mPos;

};

 std::vector< float > ProfilerBars::mCPUTimeSteps;

//-----------------------------------------------------------------------------


DebugLayer::DebugLayer() :
	SimpleUIListener(),
	IConfigSlidersListener(),
	GameMouseListener( GameInputIndex::Debug_Inspector, "DebugLayer" ),
	poro::IKeyboardListener(),
	mEventMgr( NULL ),
	mEntityMgr( NULL ),
	mDebugSprite( NULL ),
	mLoadEntityEnabled( true ),
	mConfigSliders( NULL ),
	mConfigSlidersSizeY( 0 ),
	mConfigScrollbar( NULL ),
	mInspectorEntity( NULL ),
	mExCursorVisibility( true ),
	mProfilerViewer( NULL ),
	mProfilerBars( NULL ),
	mScreenshotter( NULL )
{
	Init();
}
//.............................................................................
	
DebugLayer::DebugLayer( SGF::EventManager* eventMgr, SGF::EntityManager* entityMgr ) :
	SimpleUIListener(),
	IConfigSlidersListener(),
	GameMouseListener( GameInputIndex::Debug_Inspector, "DebugLayer" ),
	poro::IKeyboardListener(),
	mEventMgr( NULL ),
	mEntityMgr( NULL ),
	mDebugSprite( NULL ),
	mLoadEntityEnabled( true ),
	mConfigSliders( NULL ),
	mConfigSlidersSizeY( 0 ),
	mConfigScrollbar( NULL ),
	mInspectorEntity( NULL ),
	mExCursorVisibility( true ),
	mProfilerViewer( NULL ),
	mProfilerBars( NULL ),
	mScreenshotter( NULL )
{
	Init();
	SetManagers( eventMgr, entityMgr );
}
//.............................................................................

DebugLayer::~DebugLayer()
{
	Exit();
}
//-----------------------------------------------------------------------------

void DebugLayer::SetManagers( SGF::EventManager* eventMgr, SGF::EntityManager* entityMgr )
{
	mEventMgr = eventMgr;
	mEntityMgr = entityMgr;
}

//-----------------------------------------------------------------------------

void DebugLayer::Init()
{
	// poro::DefaultApplication::Init();

	Poro()->GetKeyboard()->AddKeyboardListener( this );

	mScreenshotter.reset( new Screenshotter );
	// mProfilerViewer.reset( new SimpleProfilerViewer );
	mDebugSprite = new as::Sprite;

	mConfigSliders = NULL;
	mInspectorEntity = NULL;
	mConfigScrollbar = NULL;
}
//.............................................................................

void DebugLayer::Exit()
{
	// poro::DefaultApplication::Exit();
	Poro()->GetKeyboard()->RemoveKeyboardListener( this );


	mScreenshotter.reset( NULL );
	mProfilerViewer.reset( NULL );

	delete mConfigSliders;
	mConfigSliders = NULL;
	delete mConfigScrollbar;
	mConfigScrollbar = NULL;
	delete mDebugSprite;
	mDebugSprite = NULL;
}
//-----------------------------------------------------------------------------

void DebugLayer::Update( float dt )
{
	if( mScreenshotter.get() ) mScreenshotter->Update( dt );
	if( mProfilerViewer.get() ) mProfilerViewer->Update( dt );
	if( mProfilerBars.get() )	mProfilerBars->Update( dt );

	// rotation hack
#ifndef DEBUG_LAYER_DONT_USE_COMPONENTS
	if( mInspectorEntity && mInspectorEntity->HasTransform() && mInspectorTransformRotation != mExInspectorTransformRotation )
	{
		mInspectorEntity->GetTransform()->R.Set( mInspectorTransformRotation );
		mExInspectorTransformRotation = mInspectorTransformRotation;
	}
#endif
}
//-----------------------------------------------------------------------------

void DebugLayer::Draw( poro::IGraphics* graphics )
{
	// --- debug drawing ---
	if( mConfigSliders ) as::DrawSprite( mConfigSliders, graphics );
	if( mDebugSprite ) as::DrawSprite( mDebugSprite, graphics );
	
	if( mProfilerBars.get() ) as::DrawSprite( mProfilerBars.get(), graphics );
	if( mScreenshotter.get() ) mScreenshotter->Draw( graphics );
	if( mProfilerViewer.get() ) mProfilerViewer->Draw( graphics, NULL, as::Transform() );
}
//-----------------------------------------------------------------------------

void DebugLayer::SetEntity( SGF::Entity* entity )
{
	mInspectorEntity = entity;
	RecreateInspector();
}

SGF::Entity* DebugLayer::GetEntity() const 
{
	return mInspectorEntity;
}

//-----------------------------------------------------------------------------

void DebugLayer::OpenProfilerViewer()
{
	mProfilerViewer.reset( new SimpleProfilerViewer );
}

void DebugLayer::CloseProfilerViewer()
{
	mProfilerViewer.reset( NULL );
}

//-----------------------------------------------------------------------------

void DebugLayer::OpenProfilerBars()
{
	mProfilerBars.reset( new ProfilerBars );
	mProfilerBars->MoveTo( types::vector2( 1024 - 256.f, 0.f ) );
}

void DebugLayer::CloseProfilerBars()
{
	mProfilerBars.reset( NULL );
}

//-----------------------------------------------------------------------------

void DebugLayer::OnKeyDown( int key, poro::types::charset unicode )
{
	if( key == SDLK_ESCAPE ) Poro()->Exit();

	// add some kind of debug key filtering

	// profiler viewer
	if( key == SDLK_p && Poro()->GetKeyboard()->IsCtrlDown() && Poro()->GetKeyboard()->IsShiftDown() == false ) 
	{
		if( mProfilerViewer.get() ) 
			CloseProfilerViewer();
		else 
			OpenProfilerViewer();
	}

	if( key == SDLK_p && Poro()->GetKeyboard()->IsCtrlDown() && Poro()->GetKeyboard()->IsShiftDown() ) 
	{
		if( mProfilerBars.get() ) 
			CloseProfilerBars();
		else
			OpenProfilerBars();
	}

	if( mLoadEntityEnabled )
	{
	#ifndef DEBUG_LAYER_DONT_USE_COMPONENTS
		// saving the inspector 
		if( key == SDLK_s && Poro()->GetKeyboard()->IsCtrlDown() && mInspectorEntity ) 
		{
			std::string filename = SaveFileDialog( "data\\entities" );
			if( filename.empty() == false ) 
				SaveEntity( filename, mInspectorEntity, mEntityMgr );
		}

		// load the inspector
		if( key == SDLK_o && Poro()->GetKeyboard()->IsCtrlDown() )
		{
			std::string filename = LoadFileDialog( "data\\entities" );
			// std::string filename = "data\\entities\\test_gun.xml";
			if( filename.empty() == false ) {
				mInspectorEntity = LoadEntity( filename, mEntityMgr );
				mInspectorEntity->GetTransform()->position = mMousePositionInWorld;
				RecreateInspector();
			}
		}
	#endif
	}
}

//-----------------------------------------------------------------------------

void DebugLayer::OnKeyUp( int key, poro::types::charset unicode )
{
}

//-----------------------------------------------------------------------------

bool DebugLayer::OnMouseMove( const types::vector2& screen_position )
{
	return false;
}

//.............................................................................

bool DebugLayer::OnMouseDown( const types::vector2& pos, int button ) 
{ 
		// mouse wheels for debug
	if( ( button == GameMouseButton::WHEEL_UP ||
		button ==  GameMouseButton::WHEEL_DOWN ) &&
		mConfigSliders &&
		mConfigScrollbar )
	{
		// check if mouse cursor is on top of the config sliders
		if( ceng::math::IsPointInsideAABB( types::vector2( pos ), mConfigSliders->GetPos(), mConfigSliders->GetPos() + mConfigSliders->GetBoxSize() + types::vector2( 0, 50 ) ) )
		{
			float direction = button == GameMouseButton::WHEEL_UP? 1.f : -1.f;

			float slider_value = mConfigScrollbar->GetSliderValue();

			float height = Poro()->GetInternalHeight() - mConfigSlidersSizeY;
			float value_change = 100.f / height;

			slider_value = ceng::math::Clamp( slider_value + direction * value_change, 0.f, 1.f );

			this->OnSlideTo( mConfigScrollbar, slider_value );

			return true;
		}
	}

	return false;
}

//-----------------------------------------------------------------------------

void DebugLayer::SetConfig( ConfigSliders* config )
{
	if( mConfigSliders != config )
	{
		delete mConfigSliders;
		mConfigSliders = NULL;
	}

	mConfigSliders = config;

	if( mConfigSliders )
	{
		mExCursorVisibility = Poro()->GetMouse()->IsCursorVisible();
		Poro()->GetMouse()->SetCursorVisiblity( true );
	}
	else
	{
		Poro()->GetMouse()->SetCursorVisiblity( mExCursorVisibility );
	}
}

ConfigSliders*	DebugLayer::GetConfig()
{
	return mConfigSliders;
}

//-----------------------------------------------------------------------------

void DebugLayer::OnSlideEnd( as::Sprite* sprite, float value )
{
	// std::cout << "Name: " << sprite->GetName() << std::endl;
}
//.............................................................................

void DebugLayer::OnSlideTo( as::Sprite* sprite, float value )
{
	if( sprite == mConfigScrollbar && mConfigSliders )
	{
		// mConfigSlidersSizeY
		float y_pos = ceng::math::Lerp( 0.f, Poro()->GetInternalHeight() - mConfigSlidersSizeY, value );
		mConfigSliders->SetY( ceng::math::Round( y_pos ) );
		mConfigScrollbar->SetSliderValue( value );
	}
	else
	{
		// std::cout << "Sliding values to: " << value << std::endl;
	}
}
//.............................................................................

void DebugLayer::OnSliderChange()
{

}
//-----------------------------------------------------------------------------

void DebugLayer::OnRemoveComponent( void* extra_data )
{
#ifndef DEBUG_LAYER_DONT_USE_COMPONENTS
	if( mConfigSliders )
	{
		SGF::Component* component_to_remove = static_cast< SGF::Component* >( extra_data );
		mEntityMgr->RemoveComponent( mInspectorEntity, component_to_remove );
		RecreateInspector();
	}
#endif
}
//.............................................................................

void DebugLayer::OnAddComponent( const std::string& component_name )
{
#ifndef DEBUG_LAYER_DONT_USE_COMPONENTS
	if( mConfigSliders && mInspectorEntity )
	{
		SGF::Component* component = SGF::component::create( component_name );
		if( component == NULL ) 
		{
			std::cout << "ERROR! Couldn't create component: " << component_name << std::endl;
		}
		else
		{
			mEntityMgr->InsertComponent( mInspectorEntity, component );
			RecreateInspector();
		}
	}
#endif
}

//-----------------------------------------------------------------------------

void DebugLayer::RecreateInspector()
{
#ifndef DEBUG_LAYER_DONT_USE_COMPONENTS
	bool slide_into_place = false;
	if( mConfigSliders ) 
	{
		slide_into_place = true;

		delete mConfigSliders;
		mConfigSliders = NULL;

		Poro()->GetMouse()->SetCursorVisiblity( mExCursorVisibility );
	}


	// CREATE INSPECTOR
	if( mInspectorEntity == NULL ) return;

	mExCursorVisibility = Poro()->GetMouse()->IsCursorVisible();
	Poro()->GetMouse()->SetCursorVisiblity( true );

	SGF::Entity* e = mInspectorEntity;
	ConfigSliders* config_sliders = new ConfigSliders( this );
	mConfigSliders = config_sliders;

	std::vector< SGF::Component* > components;
	mEntityMgr->GetAllComponents( e, components );

	config_sliders->AddHeaderText( "Entity ( " + ceng::CastToString( e->GetID() ) + " )" );


	

	// --- transform, should we force this always?
	if( e->GetTransform() ) 
	{
		config_sliders->AddComponentHeader( "Transform", true, (void*)e->GetTransform() );
		config_sliders->mCanCreateLines = false;

		config_sliders->AddVariable( e->GetTransform()->position.x, "position.x", MetaData( ceng::math::Min( 0.f, 0.25f * e->GetTransform()->position.x ), ceng::math::Max( 1024.f, 4.f * e->GetTransform()->position.x ) ) );
		config_sliders->AddVariable( e->GetTransform()->position.y, "position.y", MetaData( ceng::math::Min( 0.f, 0.25f * e->GetTransform()->position.y ), ceng::math::Max( 1024.f, 4.f * e->GetTransform()->position.y ) ) );

		config_sliders->AddVariable( e->GetTransform()->scale.x, "scale.x", MetaData( ceng::math::Min( 0.f, 0.25f * e->GetTransform()->scale.x ), ceng::math::Max( 10.f, 4.f * e->GetTransform()->scale.x ) ) );
		config_sliders->AddVariable( e->GetTransform()->scale.y, "scale.y", MetaData( ceng::math::Min( 0.f, 0.25f * e->GetTransform()->scale.y ), ceng::math::Max( 10.f, 4.f * e->GetTransform()->scale.y ) ) );

		mInspectorTransformRotation = e->GetTransform()->R.GetAngle();
		mExInspectorTransformRotation = mInspectorTransformRotation;
		config_sliders->AddVariable( mInspectorTransformRotation, "rotation", MetaData( -ceng::math::pi, ceng::math::pi ) );

		config_sliders->mCanCreateLines = true;
		config_sliders->CreateLine();
	}


	// --- components
	for( std::size_t i = 0; i < components.size(); ++i ) 
	{
		if( components[ i ] ) 
		{
			config_sliders->AddComponentHeader( components[ i ]->GetName(), true, (void*)components[i] );
			config_sliders->mCanCreateLines = false;
			components[ i ]->GetConfigSliders( config_sliders, this );
			config_sliders->mCanCreateLines = true;
			config_sliders->CreateLine();
		}
	}

	// --- Add components?
	std::vector< std::string > names_of_components;
	SGF::ComponentTypeManager::Instance()->GetNamesOfAllComponents( names_of_components );
	std::reverse( names_of_components.begin(), names_of_components.end() );
	config_sliders->AddAddComponent( "Add Component", names_of_components );

	float slider_value = 0.f;
	if( mConfigScrollbar ) 
	{
		slider_value = mConfigScrollbar->GetSliderValue();
		delete mConfigScrollbar;
		mConfigScrollbar = NULL;
	}

	// scroll bar?
	if( config_sliders->GetBoxSize().y + (float)names_of_components.size() * 30.f > Poro()->GetInternalHeight() )
	{
		

		float size = config_sliders->GetBoxSize().y + (float)names_of_components.size() * 30.f + 30.f;
		mConfigSlidersSizeY = size;

		SimpleSlider* slider = new SimpleSlider( this );
		mConfigScrollbar = slider;
		
		types::vector2 position;
		position.x = config_sliders->GetPos().x + config_sliders->GetBoxSize().x - 6.f;
		position.y = 100;

		slider->MoveTo( position );

		// --- image
		
		as::Sprite* slider_image = as::LoadSprite( "data/debug/sui_scrollbar.png" );
		slider_image->SetCenterOffset( 0.5f * slider_image->GetTextureSize() );
		slider_image->SetName( "slider_image" );

		// -- /image

		slider->addChild( slider_image );
		slider->SetName( "inspector_scrollbar" );
		slider->mStartPos = types::vector2( position.x, 0.5f * slider_image->GetTextureSize().y );
		slider->mEndPos = types::vector2( position.x, Poro()->GetInternalHeight() - 0.5f * slider_image->GetTextureSize().y );
		slider->SetSliderValue( slider_value );

		mDebugSprite->addChild( slider );
		OnSlideTo( slider, slider->GetSliderValue() );

		// std::cout << "ConfigSliders size: " << config_sliders->GetBoxSize().x << ", " << config_sliders->GetBoxSize().y << std::endl;
	}

	// tweeen that shit
	if( slide_into_place && mConfigSliders )
	{
		types::vector2 target = mConfigSliders->GetPos();
		mConfigSliders->MoveTo( target + types::vector2( 350.f, 0 ));
		GTweenSpriteTo( mConfigSliders, target, 0.3f, ceng::easing::Cubic::easeOut  );
	}
#endif
}
//-----------------------------------------------------------------------------

#include "mouse_clicker.h"

// #define IMPL_DEV_BUILD

#include <sdl.h>

#include <game_utils/tween/tween.h>
#include <game_utils/drawlines/drawlines.h>

#include <utils/math/cstatisticshelper.h>
#include <utils/vector_utils/vector_utils.h>

#include "gameplay_utils/game_mouse.h"
#include "misc_utils/debug_layer.h"
#include "misc_utils/simple_profiler.h"
#include "particle_effects.h"

#include "misc_utils/font_creator.h"

// ----------------------------------------------------------------------------

const float window_w = 2048;
const float window_h = 1280;
const float max_time = 5.f;

jesus::ParticleEffects* mParticleEffects = NULL;

int mHighscore = 0;

bool mStarted = false;
float mStartTime = 0;
int mMouseClicks = 0;
float mLastTimePressed = 0;
float mTargetScale = ( ( 2.f + 1 * 0.3f ) * 0.085f );

std::vector< float > mGraph;

ceng::math::CStatisticsHelper< float > mButtonDelay;
float mButtonDelayNow = 0;

float GetTime()
{
	float t = 0;
	if( mStarted ) 
		t = Poro()->GetTime() - mStartTime;

	if( t > max_time ) 
		t = max_time;

	return t;
}

void OnMouseDown()
{
	if( mStarted == false )
	{
		mStarted = true;
		mStartTime = Poro()->GetTime();
		mMouseClicks = 0;
		mLastTimePressed = Poro()->GetTime();
		mButtonDelay.Reset();
	}
	else
	{

		float mButtonPressTook = Poro()->GetTime() - mLastTimePressed;
		mLastTimePressed = Poro()->GetTime();

		float m_secs = (float)(mButtonPressTook * 1000.f);
		mButtonDelayNow = m_secs;

		if( mMouseClicks >= (int)mGraph.size() )
			mGraph.resize( 2 * mGraph.size() );

		mGraph[ mMouseClicks ]= mButtonDelayNow;
		mMouseClicks++;

		mButtonDelay += m_secs;

	}
}

MouseClicker::MouseClicker()
{
}


void MouseClicker::Exit()
{
	std::fstream fout( "highscore.txt", std::ios::out );
	fout << mHighscore << std::endl;
	fout.close();

	mDebugLayer.reset( NULL );
}

//-----------------------------------------------------------------------------

void MouseClicker::Init()
{
	DefaultApplication::Init();
	Poro()->GetGraphics()->SetFillColor( poro::GetFColor( 248.f / 255.f, 245.f / 255.f, 236.f / 255.f, 1.f ) );

	Poro()->GetMouse()->HideCursor();
	// Poro()->SetPrintFramerate( false );

	// CreateFont( "data/fonts/Ubuntu-C.ttf", 1200, "data/fonts/ubuntu.xml", "data/fonts/ubuntu.png" );

	std::vector< std::string > lines;
	ceng::VectorLoadFromTxtFile( lines, "highscore.txt" );
	if( lines.empty() == false && lines[0].empty() == false )
		mHighscore = ceng::CastFromString< int >( lines[0] );

	GameMouse::GetSingletonPtr()->Init();

	mSpriteContainer = new as::Sprite;
	mSlider = as::LoadSprite( "data/slider.png" );
	mSlider->SetSize( (int)window_w, (int)window_h );
	mSlider->MoveTo( types::vector2( 0, 0 ) );
	mSpriteContainer->addChild( mSlider );

	mScore = as::LoadTextSprite( "data/fonts/ubuntu.xml" );
	mSpriteContainer->addChild( mScore );

	SetScore( 0 );

	// REMOVE FOR RELEASE
#ifdef IMPL_DEV_BUILD
	mDebugLayer.reset( new DebugLayer );
#endif

	mParticleEffects = new jesus::ParticleEffects;
	mParticleEffects->SetScale( 2, 2 );
	mSpriteContainer->addChild( mParticleEffects );


	mGraph.resize( 256, 0 );
}

// ----------------------------------------------------------------------------

void MouseClicker::SetScore( int score )
{
	// score = score + 1;
	mScore->SetText(ceng::CastToString( score ));
	mScore->SetCenterOffset( 0.5f * mScore->GetTextureSize() );
	mScore->MoveTo( types::vector2( 0.5f * window_w, 0.5f * window_h ) );
	mScore->SetColor( 1, 1, 1 );

	// score = 66;
	float scale = ( 2.f + score * 0.3f ) * 0.085f;
	mTargetScale = scale;
	
	// mScore->SetScale( scale * 1.1f, scale * 1.1f );

}

void MouseClicker::Clear()
{
}

// ----------------------------------------------------------------------------

void MouseClicker::Update( float dt )
{
	UpdateGTweens( dt );

	if( mDebugLayer.get() ) 
		mDebugLayer->Update( dt );

	// MouseButtonDown(poro::types::vec2(), 1);
	// UpdateSlider()
	float slider_pos = 0;
	if( mStarted )
	{
		float time = Poro()->GetTime() - mStartTime;
		float t = time / max_time;
	
		if( time > 6.5f )
		{
			time = ceng::math::Min( 7.f, time );
			t = 7.f - time;
			t /= 0.5f;
			t = ceng::easing::Quad::impl_easeInOut(t, 0, 1, 1);
		}

		slider_pos = t * window_w;
		mSlider->SetX( slider_pos );
	}

	/*if( mStarted && Poro()->GetTime() - mStartTime >= 6.5f )
	{
		GTweenSpriteTo( mSlider, types::vector2(0,0), 0.5f, ceng::easing::Quad::easeOut ); 
	}*/

	// UpdateScore
	if( mScore )
	{
		// color
		std::vector< float > color = mScore->GetColor();
		for( int i = 0; i < 3; ++i )
			color[i] *= 0.9f;
		mScore->SetColor( color );

		float scale = mScore->GetScaleX();
		float new_scale = scale + 0.1f * ( mTargetScale - scale );
		mScore->SetScale( new_scale, new_scale );
	}

	if( mSpriteContainer )
		mSpriteContainer->Update( dt );



	GameMouse::GetSingletonPtr()->OnFrameEnd();

	/*if( mStarted && ( Poro()->GetTime() - mStartTime ) < 5.f && ceng::Random( 0, 100 ) < 8 )
		OnMouseDown();*/

}

// ----------------------------------------------------------------------------

void MouseClicker::Draw( poro::IGraphics* graphics )
{ 
	SetLineWidth( 3.f );

	SPROFILE( "MouseClicker::Draw()" );

	if( mDebugLayer.get() ) 
		mDebugLayer->Draw( graphics );


	SetLineWidth( 2.f );

	// float how high
	if( mHighscore > 0 )
	{
		float highscore_t = 5.f / mHighscore;
		highscore_t *= 1000.f;

		DrawLine( graphics, 
			types::vector2( 0, window_h - ( highscore_t * 2.0f ) ),
			types::vector2( window_w, window_h - ( highscore_t * 2.0f ) ),
			poro::GetFColor( 0.15f, 0.15f, 0.15f, 0.05f ) );
	}

	SetLineWidth( 3.f );
	for( int i = 1; i < mMouseClicks; ++i )
	{
		float step = (float)window_w / (float)(mMouseClicks - 1);
		int j = i - 1;
		DrawLine( graphics, 
			types::vector2( j * step, window_h - ( mGraph[ j ] * 2.0f ) ),
			types::vector2( i * step, window_h - ( mGraph[ i ] * 2.0f ) ),
			poro::GetFColor( 0.75, 0.75, 0.75 ) );
	}


	if( mSpriteContainer )
		as::DrawSprite( mSpriteContainer, graphics );

	DrawHersheyText( graphics, "highscore: " + ceng::CastToString( mHighscore ),  
		types::vector2( window_w - 520, 50 ), 48.f, poro::GetFColor( 1, 1, 1, 1 ) );

#ifdef IMPL_DEV_BUILD

	float t = 0;
	if( mStarted ) 
		t = Poro()->GetTime() - mStartTime;

	if( t > 5.f ) 
		t = 5.0f;

	t = 5.f - t;

	DrawHersheyText( graphics, ceng::CastToString( t ),  
		types::vector2( 750, 50 ), 48.f, poro::GetFColor( 1, 1, 1, 1 ) );

	// draw statictis
	DrawHersheyText( graphics, ceng::CastToString( mButtonDelayNow ) + " ms",  
		types::vector2( 1550, 650 - ( mButtonDelayNow * 2.f ) ), 48.f, poro::GetFColor( 1, 1, 1, 1 ) );
	
	DrawHersheyText( graphics, "min: " + ceng::CastToString( mButtonDelay.GetMin() ) + " ms",  
		types::vector2( 1550, 250 ), 48.f, poro::GetFColor( 1, 1, 1, 1 ) );


#endif
	// DrawLine( graphics, types::vector2( 0, 1000.f - 100.f * 2 ), types::vector2( 2000.f, 1000.f - 100.f * 2 ), poro::GetFColor( 0, 1,0, 0.5f ) );
	// DrawLine( graphics, types::vector2( 0, 1000.f ), types::vector2( 2000.f, 1000.f ), poro::GetFColor( 1, 0,0 ) );
	
}

// ----------------------------------------------------------------------------

void MouseClicker::MouseMove(const poro::types::vec2& p)
{
	if (mDebugLayer.get())
		mDebugLayer->mMousePositionInWorld = types::vector2(p);
}

void MouseClicker::MouseButtonDown(const poro::types::vec2& p, int button)
{
	if( mStarted == false || ( Poro()->GetTime() - mStartTime ) < 5.f )
	{
		OnMouseDown();
		SetScore( mMouseClicks );
	}

	if( mStarted && ( Poro()->GetTime() - mStartTime ) > 5.f )
	{
		// game ended
		if( mMouseClicks > mHighscore )
		{
			mHighscore = mMouseClicks;
	
			if( mParticleEffects )
			{
				mParticleEffects->DoGoalEffect( types::vector2( 0, ceng::Randomf( 0.2f, 0.3f ) * window_h ), types::vector2( 0, 0 ) );
				mParticleEffects->DoGoalEffect( types::vector2( 0.5f * window_w, ceng::Randomf( 0.2f, 0.3f ) * window_h ), types::vector2( 0, 0 ) );
			}

			// save highscore
			std::fstream fout( "highscore.txt", std::ios::out );
			fout << mHighscore << std::endl;
			fout.close();

		}
	}

	if( mStarted && ( Poro()->GetTime() - mStartTime ) > 7.f )
	{
		mStarted = false;
		OnMouseDown();
		SetScore( mMouseClicks );
	}
}

void MouseClicker::MouseButtonUp(const poro::types::vec2& pos, int button)
{
}

//=============================================================================

void MouseClicker::OnKeyDown( int key, poro::types::charset unicode )
{
	if( key == 27 ) 
		Poro()->Exit();

}

//-----------------------------------------------------------------------------


void MouseClicker::OnKeyUp( int key, poro::types::charset unicode )
{
}

//=============================================================================


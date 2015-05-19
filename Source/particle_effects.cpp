#include "particle_effects.h"

#include <poro/poro.h>
#include <utils/random/random.h>
#include <utils/color/color_utils.h>
#include <game_utils/actionscript/actionscript.h>


namespace jesus {

namespace config 
{
	const float screen_w = 1024.f;
	const float screen_h = 768;
}

namespace {

struct ConfettiRotate : public IParticleHack
{
	ConfettiRotate() { mExtraT = ( ceng::math::pi * 2 * 4.f ); mExtraStart = 0; }

	bool FreeMe() const { return false; }

	void SetExtraT( float revolutions )
	{
		mExtraT = ( ceng::math::pi * 2 * revolutions );
	}

	void SetExtraStart( float t ) 
	{
		mExtraStart = t;
	}

	void Update( CParticle* particle, float dt )
	{
		if( particle->myTimeNow < 0 ) return;

		float t = ( particle->myTimeNow / particle->myLifeTime ) * mExtraT;
		
		particle->mySprite->SetScaleX( cos( t + mExtraStart ) * particle->mySprite->GetScaleY() );
	}

	float mExtraT;
	float mExtraStart;
};

// std::vector< ConfettiRotate* > static_confetti_rotates;

ConfettiRotate static_confetti_rotate;

//-----------------------------------------------------------------------------

poro::types::Uint32 confetti_colors[] = {
	0x9932FF,
	0x00CCCC,
	0x99CC00,
	0xFFFF00,
	0xFF0000
};

const int confetti_colors_count = 5;

//-----------------------------------------------------------------------------

} // end of anonymous namespace

//-----------------------------------------------------------------------------

ParticleEffects::ParticleEffects() :
	mParticleFactory()
{

}

ParticleEffects::~ParticleEffects()
{
	// release mParticleSprite?
}
//-----------------------------------------------------------------------------

void ParticleEffects::Clear()
{
	mParticleFactory.Clear();
}
//-----------------------------------------------------------------------------

void ParticleEffects::Update( float dt )
{
	mParticleFactory.Update( dt );
}
//-----------------------------------------------------------------------------

namespace {
	types::vector2 RandomVector( float min = 0.f, float max = 1.f ) 
	{
		return types::vector2( ceng::Randomf( min, max ), ceng::Randomf( min, max ) );
	}
} // end of anonymous namespace


void ParticleEffects::DoGoalEffect( const types::vector2& screen_pos, const types::vector2& velocity )
{
	// sfx
	//----
	// PlayEffect( SfxLibrary::GetSingletonPtr()->GetSfxFile( "confetti" ), 1.2f );
	// PlayEffect( SfxLibrary::GetSingletonPtr()->GetSfxFile( "kids" ) );

	//---

	int count = ceng::Random( 75, 120 );
	float force = 15.f;
	for( int i = 0; i < count; ++i ) 
	{
		types::vector2 position = screen_pos + RandomVector( -23.f, 23.f );
		position.y = ceng::math::Clamp( position.y, 150.f, config::screen_h );
		// position.Set( 0.5f * config::screen_w, 0.5f * config::screen_h );
		as::Sprite* confetti_sprite = GetRandomConfettiSprite();
		// confetti_sprite->SetColor( ceng::Randomf( 0.75f, 1.f ), ceng::Randomf( 0.75f, 1.f ), 0.f );
		float scale = ceng::Randomf( 0.35f, .75f );
		confetti_sprite->SetScale(scale, scale );
		confetti_sprite->MoveCenterTo( position );

		ceng::types::color c = ceng::GetRandomColor( confetti_colors, confetti_colors_count );
		confetti_sprite->SetColor( c.GetRf(), c.GetGf(), c.GetBf() );

		types::vector2 vel = types::vector2( 0.5f * config::screen_w, 0 ) - screen_pos;
		vel = vel.Normalize();
		vel.y = -0.75f;
		vel = vel.Normalize();
		vel = vel.Rotate( ceng::Randomf( -0.3f, 0.3f ) );

		vel *= force * 100.f * ceng::Randomf( .75f, 1.0f ) * 2.f;

		// vel.Set( 0, 0 );
		float t = 1.5f * ceng::Randomf( 0.75f, 1.54f ) + ceng::Randomf( 0, 1.25f );
		CParticle* confetti_particle = mParticleFactory.NewParticle( confetti_sprite, 
			t, 
			vel, 
			0 );
		confetti_particle->myTimeNow = ceng::Randomf( -0.05f, 0.f );
		confetti_particle->myGravity.Set( 0, ceng::Randomf( 450, 750 ) );
		confetti_particle->myVelocitySlowDown = ceng::Randomf( 4.f, 6.f );
		confetti_particle->myColorChanges[ 3 ] = -1.f / t;
		confetti_particle->myRotationVelocity = 20.f * ceng::Randomf( -0.2f, 0.2f );
		confetti_particle->myUseVelocityAsRotation = false;

		confetti_particle->myParticleHacks.push_back( &static_confetti_rotate );
		// confetti_particle->myParticleHacks.push_back( static_confetti_rotates[ ceng::Random( 0, static_confetti_rotates.size() - 1 ) ] );

		// confetti_particle->myScaleVel.Set( -scale / t, -scale / t );
	}

}
///////////////////////////////////////////////////////////////////////////////

as::Sprite* ParticleEffects::GetRandomConfettiSprite()
{
	std::stringstream ss;
	ss << "data/particles/confetti" << ceng::Random( 1, 5 ) << ".png";

	as::Sprite* result = as::LoadSprite( ss.str() );

	cassert( result );
	if( result ) 
	{
		addChild( result );
		result->SetCenterOffset( types::vector2( result->GetTextureSize().x * 0.5f, 0.5f * result->GetTextureSize().y ) );
		result->SetClearTweens( false );
	}

	return result;
}


//-----------------------------------------------------------------------------

} // end of namespace jesus
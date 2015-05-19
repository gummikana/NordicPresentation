#include "particle_effects.h"

#include <poro/poro.h>
#include <utils/random/random.h>
#include <game_utils/actionscript/actionscript.h>

#include "../../jesus_types.h"
#include "../../csfx.h"

#include "../main_task.h"
#include "sfx_library.h"

namespace jesus {

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

void ParticleEffects::DoJetpackParticle( const types::vector2& p_position, float force, const types::vector2& vel )
{
	force = 1.5f;
	int i = 0;
	int count = 1;
	as::Sprite* smoke_sprite = GetRandomJetpackSprite();

	float t = ceng::Randomf( 0.15f, 0.45f );
	//smoke_sprite->SetColor( 0.9f, 0.9f, 0.9f );

	float scale = ceng::Randomf( 1.0f, 2.0f ) * ceng::math::Square( 0.5f * force );
	scale = 0.5f * scale;
	smoke_sprite->SetScale( scale, scale );
	
	types::vector2 position = p_position;
	smoke_sprite->MoveTo( position );

	float angle = ((float) i) / ((float) count) * ceng::math::pi * 2;
		
	types::vector2 velocity = vel;
	velocity *= force * 100.f * ceng::Randomf( .90f, 1.0f );
	velocity.y += 400.f;
	
	CParticle* smoke_particle = mParticleFactory.NewParticle( smoke_sprite, 
		t, 
		velocity, 
		ceng::Randomf(0.f, 360.f));

	smoke_particle->myVelocitySlowDown = .97f;
	smoke_particle->myColorChanges[ 3 ] = -1.0f / t;
	smoke_particle->myScaleVel.Set( -scale / t, -scale / t );
	smoke_particle->myRotationVelocity = (ceng::Random(0, 1) == 0 ? 60.f : -60.f );
}

void ParticleEffects::DoExplosion( const types::vector2& p_position, float force )
{
	std::cout << "DoExplosion: " << force << std::endl;
	int count;
	count = (int)( force * ( ceng::Randomf( 25.f, 35.f ) ) );
	for( int i = 0; i < count; ++i )
	{
		
		as::Sprite* smoke_sprite = GetRandomSmokeSprite();

		float t = ceng::Randomf( 0.25f, 0.75f ) * force;

		smoke_sprite->SetColor( 0.8f, 0.2f, 0.0f );

		float scale = ceng::Randomf( 0.5f, 2.0f ) * ceng::math::Square( 0.5f * force );
		smoke_sprite->SetScale( scale, scale );
		
		types::vector2 position = p_position;
		smoke_sprite->MoveTo( position );

		float angle = ((float) i) / ((float) count) * ceng::math::pi * 2;
			
		types::vector2 velocity = types::vector2(sin(angle), cos(angle));
		velocity *= force * 100.f * ceng::Randomf( .75f, 1.0f );
		
		if(ceng::Randomf(0.f, 1.f) < .1f) velocity *= .1f;

		CParticle* smoke_particle = mParticleFactory.NewParticle( smoke_sprite, 
			t, 
			velocity, 
			ceng::Randomf( 3.f, 6.f ) );

		smoke_particle->myVelocitySlowDown = .95f;
		
		smoke_particle->myColorChanges[ 0 ] = 0.2f / t;
		smoke_particle->myColorChanges[ 1 ] = 0.8f / t;
		smoke_particle->myColorChanges[ 2 ] = 1.0f / t;

		smoke_particle->myScaleVel.Set( -scale / t, -scale / t );
	}

	count = (int)( force * ceng::Randomf( 25, 50 ) );

	for( int i = 0; i < count; ++i )
	{
		types::vector2 position = p_position + force * RandomVector( -23.f, 23.f );
		as::Sprite* spark_sprite = GetRandomSparkSprite();
		spark_sprite->SetColor( ceng::Randomf( 0.75f, 1.f ), ceng::Randomf( 0.75f, 1.f ), 0.f );
		float scale = ceng::Randomf( 0.75f, 1.25f );
		spark_sprite->SetScale(scale, scale );
		spark_sprite->MoveCenterTo( position );

		float angle = ((float) i) / ((float) count) * ceng::math::pi * 2;
		types::vector2 velocity = types::vector2(sin(angle), cos(angle));
		velocity *= force * 100.f * ceng::Randomf( .75f, 1.0f ) * 2.f;

		float t = 2 * ( -velocity.y / 600.f );
		t += ceng::Randomf( -0.25f, 0.25f );
		CParticle* spark_particle = mParticleFactory.NewParticle( spark_sprite, 
			t, 
			velocity, 
			0 );
		spark_particle->myTimeNow = ceng::Randomf( 0, 1 ) * -(.1f + 0.01f * (float)i); 
		spark_particle->myGravity.Set( 0, 600.f );
		spark_particle->myUseVelocityAsRotation = true;
		spark_particle->myScaleVel.Set( -scale / t, -scale / t );
	}

}

//-----------------------------------------------------------------------------

void ParticleEffects::DoPhysicsShaking( const types::vector2& screen_pos, const types::vector2& p_velocity, float shakiness )
{
	// b	ig parts.... 1000 - 1500 ... around that
	float vol_force = p_velocity.Length() * shakiness;
	if( p_velocity.y < -300.f && vol_force > 850.f )
	{
		float shake_force = ( ( vol_force - 850.f ) / 650.f ) * 4.5f;
		ShakeScreen( shake_force, 3.0f ); 
		std::cout << "ShakeForce: " << shake_force << std::endl;
	}
}

void ParticleEffects::DoWheelGroundCollision( const types::vector2& p_position, const types::vector2& p_velocity, float direction_x )
{
	if( p_position.x < 0 || p_position.x > config::screen_w ) return;

	// play effect if velocity is greater than threshold
	float vol_force = p_velocity.Length();


	if( vol_force >= 650.f ) 
	{
		static float last_time_played = 0;
		
		if( ( poro::IPlatform::Instance()->GetUpTime() - last_time_played ) < 30 / 1000.f ) {
			return;
		}

		last_time_played = poro::IPlatform::Instance()->GetUpTime();

		float volume = ceng::math::Clamp( ( vol_force - 650.f ) / 50.f, 0.f, 1.f );
		if( volume <= 0 && ceng::Randomf( 0, 100 ) < 50.f ) 
			volume = ceng::Randomf( 0.01f, 0.4f );

		volume = 0.25f * volume;

		if( volume > 0 ) 
		{
			std::string effect_name = SfxLibrary::GetSingletonPtr()->GetSfxFile( "tire" );
			PlayEffect( effect_name, volume );
		}


		float force = 0.5f * ( p_velocity.Length() / 650.f );
		int count = (int)( force * ceng::Randomf( 10, 20 ) );
		
		
		for( int i = 0; i < count; ++i )
		{
			types::vector2 position = p_position + force * RandomVector( -23.f, 23.f );
			as::Sprite* spark_sprite = GetRandomSparkSprite();
			spark_sprite->SetColor( ceng::Randomf( 0.95f, 1.f ) * ( 71.f / 255.f ), ceng::Randomf( 0.95f, 1.f ) * ( 62.f / 255.f ), ceng::Randomf( 0.95f, 1.f ) * ( 53.f / 255.f ) );
			float scale = ceng::Randomf( 0.75f, 1.25f );
			spark_sprite->SetScale(scale, scale );
			spark_sprite->MoveCenterTo( position );

			// float angle = ((float) i) / ((float) count) * ceng::math::pi * 2;
			types::vector2 velocity( direction_x * ceng::Randomf( 200, 500 ), ceng::Randomf( -200, -500 ) );
			velocity = velocity.Rotate( ceng::Randomf( -0.2f, 0.2f ) );
			velocity *= force * ceng::Randomf( .75f, 1.0f );

			float t = 2 * ( -velocity.y / 600.f );
			t += ceng::Randomf( -0.25f, 0.25f );
			CParticle* spark_particle = mParticleFactory.NewParticle( spark_sprite, 
				t, 
				velocity, 
				0 );
			spark_particle->myTimeNow = ceng::Randomf( 0, 1 ) * -(.1f + 0.01f * (float)i); 
			spark_particle->myGravity.Set( 0, 600.f );
			spark_particle->myUseVelocityAsRotation = true;
			spark_particle->myScaleVel.Set( -scale / t, -scale / t );
		}
	}
}
//-----------------------------------------------------------------------------

void ParticleEffects::DoCarToCarCollision( const types::vector2& p_position, const types::vector2& p_velocity )
{
	std::cout << "p_velocity( " << p_velocity.Length() << " ): " <<
		p_velocity.x << ", " << p_velocity.y << std::endl;

	float vol_force = p_velocity.Length();
	if( vol_force >= 50.f /*|| ceng::Randomf( 0, 2000.f ) < vol_force*/ ) 
	{
		static float last_time_played = 0;
		
		if( ( poro::IPlatform::Instance()->GetUpTime() - last_time_played ) < 30 / 1000.f ) {
			return;
		}

		last_time_played = poro::IPlatform::Instance()->GetUpTime();

		float volume = ceng::math::Clamp( ( vol_force - 50.f ) / 200.f, 0.f, 1.f );
		if( volume <= 0 && ceng::Randomf( 0, 100 ) < 50.f ) 
			volume = ceng::Randomf( 0.01f, 0.4f );

		volume = 0.75f * volume;

		if( vol_force > 750.f ) 
		{
			if( SfxLibrary::GetSingletonPtr()->GetSfxEffect( "smack" ) ) 
			{
				int effect_count = SfxLibrary::GetSingletonPtr()->GetSfxEffect( "smack" )->HowManyTimesPlayedInTheLast( 1.5f );
				if( effect_count >= 3 ) 
				{
					// just to amp the counter...
					// std::string effect_name = SfxLibrary::GetSingletonPtr()->GetSfxFile( "smack" );
					std::string effect_name = SfxLibrary::GetSingletonPtr()->GetSfxFile( "car_horn" );
					PlayEffect( effect_name, volume );

				}
				else
				{
					volume *= 0.6f;
					std::string effect_name = SfxLibrary::GetSingletonPtr()->GetSfxFile( "smack" );
					PlayEffect( effect_name, volume );

					// std::cout << "Effect: " << effect_name << std::endl;
				}
			}
		}
		else if( volume > 0 ) 
		{
			volume *= 0.9f;
			std::string effect_name = SfxLibrary::GetSingletonPtr()->GetSfxFile( "impact" );
			PlayEffect( effect_name, volume );

			// std::cout << "Effect: " << effect_name << std::endl;
		}
		
	// }
	return;

		float force = 0.5f * ( p_velocity.Length() / 650.f );
		int count = (int)( force * ceng::Randomf( 10, 20 ) );
		
		
		for( int i = 0; i < count; ++i )
		{
			types::vector2 position = p_position + force * RandomVector( -23.f, 23.f );
			as::Sprite* spark_sprite = GetRandomSparkSprite();
			spark_sprite->SetColor( ceng::Randomf( 0.95f, 1.f ) * ( 71.f / 255.f ), ceng::Randomf( 0.95f, 1.f ) * ( 62.f / 255.f ), ceng::Randomf( 0.95f, 1.f ) * ( 53.f / 255.f ) );
			float scale = ceng::Randomf( 0.75f, 1.25f );
			spark_sprite->SetScale(scale, scale );
			spark_sprite->MoveCenterTo( position );

			// float angle = ((float) i) / ((float) count) * ceng::math::pi * 2;
			types::vector2 velocity = p_velocity;
			velocity = velocity.Rotate( ceng::Randomf( -0.2f, 0.2f ) );
			velocity *= force * ceng::Randomf( .75f, 1.0f );

			float t = 2 * ( -velocity.y / 600.f );
			t += ceng::Randomf( -0.25f, 0.25f );
			CParticle* spark_particle = mParticleFactory.NewParticle( spark_sprite, 
				t, 
				velocity, 
				0 );
			spark_particle->myTimeNow = ceng::Randomf( 0, 1 ) * -(.1f + 0.01f * (float)i); 
			spark_particle->myGravity.Set( 0, 600.f );
			spark_particle->myUseVelocityAsRotation = true;
			spark_particle->myScaleVel.Set( -scale / t, -scale / t );
		}
	}
}
//-----------------------------------------------------------------------------

void ParticleEffects::DoCarGroundCollision( const types::vector2& screen_pos, const types::vector2& velocity )
{
	std::cout << "Car to ground: " << velocity.Length() << std::endl;
	return;
}
//-----------------------------------------------------------------------------

void ParticleEffects::DoGoalEffect( const types::vector2& screen_pos, const types::vector2& velocity )
{
	// sfx
	//----
	PlayEffect( SfxLibrary::GetSingletonPtr()->GetSfxFile( "confetti" ), 1.2f );
	PlayEffect( SfxLibrary::GetSingletonPtr()->GetSfxFile( "kids" ) );

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

	//---------------------------------------
	// light beams
	
	count = 15;
	for( int i = 0; i < count; ++i )
	{
		types::vector2 position = screen_pos;
		// position.x = ceng::math::Clamp( position.x, 0.f, config::screen_w );
		position.y = ceng::math::Clamp( position.y, 0.f, config::screen_h );

		// hack to see what happens
		// position.Set( 0.5f * config::screen_w, 0.5f * config::screen_h );

		as::Sprite* light_sprite = GetRandomLightbeamSprite();
		light_sprite->SetAlpha( 1.5f * ceng::Randomf( 0.1f, 0.35f ) );
		float scale_x = ceng::Randomf( 0.25f, 1.f );
		float scale_y = ceng::Randomf( 0.75f, 1.f );
		scale_x *= 0.75f * 2.f;
		scale_y *= 0.75f * 2.f;
		light_sprite->SetScale( 0 , 0 );
		light_sprite->MoveCenterTo( position );

		// float angle = ((float) i) / ((float) count) * ceng::math::pi * 2;
		types::vector2 vel = types::vector2( 0.5f * config::screen_w, 0 ) - screen_pos;
		vel = vel.Normalize();
		vel = vel.Rotate( ceng::Randomf( -0.2f, 0.2f ) );
		
		light_sprite->SetRotation( vel.Angle() - (float)ceng::math::pi * 0.5f );

		float t = .75f;
		t += ceng::Randomf( -0.25f, 0.25f );
		CParticle* light_particle = mParticleFactory.NewParticle( light_sprite, 
			t, 
			types::vector2( 0, 0 ),
			0 );

		cassert( t != 0 );
		light_particle->myScaleVel.Set( ceng::Randomf( 1.2f, 2.5f ) * scale_x / t, ceng::Randomf( 1.2f, 2.5f ) * scale_y / t );
		light_particle->myRotationVelocity = ceng::Randomf( -0.2f, 0.2f );
		light_particle->myColorChanges[ 3 ] = -( light_sprite->GetAlpha() / t );
	}
}
///////////////////////////////////////////////////////////////////////////////

as::Sprite* ParticleEffects::GetRandomSmokeSprite()
{
	std::stringstream ss;
	ss << "data/particles/smokeparticle" << ceng::Random( 1, 5 ) << ".png";

	as::Sprite* result = as::LoadSprite( ss.str() );

	if( result ) 
	{
		addChild( result );
		result->SetCenterOffset( result->GetTextureSize() * 0.5f );
		result->SetClearTweens( false );
	}

	return result;
}
//-----------------------------------------------------------------------------

as::Sprite* ParticleEffects::GetRandomSparkSprite()
{
	std::stringstream ss;
	ss << "data/particles/sparkparticle" << ceng::Random( 1, 5 ) << ".png";

	as::Sprite* result = as::LoadSprite( ss.str() );

	if( result ) 
	{
		addChild( result );
		result->SetCenterOffset( types::vector2( result->GetTextureSize().x * 0.5f, result->GetTextureSize().y * 0.5f ) );
		result->SetClearTweens( false );
	}

	return result;
}

//-----------------------------------------------------------------------------

as::Sprite* ParticleEffects::GetRandomLightbeamSprite()
{
	as::Sprite* result = as::LoadSprite( "data/particles/light_beam.png" );

	cassert( result );
	if( result ) 
	{
		addChild( result );
		result->SetCenterOffset( types::vector2( result->GetTextureSize().x * 0.5f, 0.125f * result->GetTextureSize().y ) );
		result->SetClearTweens( false );
	}

	return result;
}
//-----------------------------------------------------------------------------

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


as::Sprite* ParticleEffects::GetRandomJetpackSprite()
{
	//std::stringstream ss;
	//ss << "data/particles/smokeparticle" << ceng::Random( 1, 5 ) << ".png";
	//as::Sprite* result = as::LoadSprite( ss.str() );

	as::Sprite* result = as::LoadSprite( "data/particles/jetpackparticle.png" );

	if( result ) 
	{
		addChild( result );
		result->SetCenterOffset( result->GetTextureSize() * 0.5f );
		result->SetClearTweens( false );
	}

	return result;
}

//-----------------------------------------------------------------------------

} // end of namespace jesus
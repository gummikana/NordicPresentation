#ifndef INC_PARTICLE_EFFECT_H
#define INC_PARTICLE_EFFECT_H

#include <game_utils/actionscript/sprite.h>
#include <game_utils/particle/cparticlefactory.h>


namespace jesus {

class ParticleEffects : public as::Sprite
{
public:

	ParticleEffects();
	~ParticleEffects();
	
	//-------------------------------------------------------------------------

	void Clear();
	
	//-------------------------------------------------------------------------

	void Update( float dt );

	//-------------------------------------------------------------------------

	void DoExplosion( const types::vector2& screen_pos, float force );

	void DoWheelGroundCollision( const types::vector2& screen_pos, const types::vector2& velocity, float direction_x );
	void DoCarGroundCollision( const types::vector2& screen_pos, const types::vector2& velocity );
	void DoCarToCarCollision( const types::vector2& screen_pos, const types::vector2& velocity );

	// shakes camera if the velocity is great enough
	// type 0: normal collision
	// type 1: rock blocks collision
	// type 2: big blocks collision
	void DoPhysicsShaking( const types::vector2& screen_pos, const types::vector2& velocity, float shakiness );

	//-------------------------------------------------------------------------

	void DoJetpackParticle( const types::vector2& screen_pos, float force, const types::vector2& velocity );

	//-------------------------------------------------------------------------

	void DoGoalEffect( const types::vector2& screen_pos, const types::vector2& veloicty );

	//-------------------------------------------------------------------------

	as::Sprite* GetRandomSmokeSprite();
	as::Sprite* GetRandomSparkSprite();
	as::Sprite* GetRandomLightbeamSprite();
	as::Sprite* GetRandomConfettiSprite();
	as::Sprite* GetRandomJetpackSprite();

	//-------------------------------------------------------------------------

	CParticleFactory mParticleFactory;

	//-------------------------------------------------------------------------

	std::vector< int > mTireSoundEffectsRandom;
	int	mTireSoundEffectsPos;

};


} // end of namespace jesus

#endif


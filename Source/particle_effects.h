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

	void DoGoalEffect( const types::vector2& screen_pos, const types::vector2& veloicty );

	//-------------------------------------------------------------------------

	as::Sprite* GetRandomConfettiSprite();

	//-------------------------------------------------------------------------

	CParticleFactory mParticleFactory;

	//-------------------------------------------------------------------------


};


} // end of namespace jesus

#endif


#include "tween_entities.h"

#include "../component_framework/Entity.h"
#include <game_utils/tween/tween_utils.h>


void GTweenClearEntityOfTweens( SGF::Entity* e )
{
	std::list< GTween* >& list_of_gtweens = ceng::CAutoList< GTween >::GetList();

	for( std::list< GTween* >::iterator i = list_of_gtweens.begin();
		i != list_of_gtweens.end(); ++i )
	{
		GTween* tween = *i;
		if( tween->HasPointer( e ) )
			tween->Kill();
	}
}

GTween* GetGTween( void* uses_pointer, const std::string& name )
{
	std::list< GTween* >& list_of_gtweens = ceng::CAutoList< GTween >::GetList();

	for( std::list< GTween* >::iterator i = list_of_gtweens.begin();
		i != list_of_gtweens.end(); ++i )
	{
		GTween* tween = *i;
		if( tween->IsDead() == false && tween->GetName() == name && tween->HasPointer( uses_pointer ) )
			return tween;
	}

	return NULL;
}


GTween* GTweenEntityTo( SGF::Entity* e, const types::vector2& pos, float time, ceng::easing::IEasingFunc& math_func, bool autokill)
{
	if( e->GetTransform()->position == pos ) 
		return NULL;

	GTween* old_tween = GetGTween( (void*)e, "entity_position" );
	if( old_tween && old_tween->GetUserData() == pos ) 
		return NULL;

	// we have old but it's going somewhere else, so let's kill it
	if( old_tween )
		old_tween->Kill();

	GTween* new_tween = new GTween( time, autokill );

	// to identify this
	new_tween->SetName( "entity_position" );
	new_tween->SetUserData( pos );
	new_tween->AddPointer( (void*)e );

	new_tween->SetFunction( math_func );
	new_tween->AddVariable( e->GetTransform()->position, pos, "position" );

	return new_tween;
}

GTween* GTweenEntityScaleTo( SGF::Entity* e, const types::vector2& scale, float time, ceng::easing::IEasingFunc& math_func, bool autokill)
{
	if( e->GetTransform()->scale == scale ) 
		return NULL;

	GTween* old_tween = GetGTween( (void*)e, "entity_scale" );
	if( old_tween && old_tween->GetUserData() == scale ) 
		return NULL;

	// we have old but it's going somewhere else, so let's kill it
	if( old_tween )
		old_tween->Kill();
	
	GTween* new_tween = new GTween( time, autokill );

	// to identify this
	new_tween->SetName( "entity_scale" );
	new_tween->SetUserData( scale );
	new_tween->AddPointer( (void*)e );


	new_tween->SetFunction( math_func );
	new_tween->AddVariable( e->GetTransform()->scale, scale, "scale" );
	new_tween->AddPointer( (void*)e );

	return new_tween;
}


GTween* GTweenEntityRotationTo( SGF::Entity* e, float rotation, float time, ceng::easing::IEasingFunc& math_func, bool autokill)
{
	if( e->GetTransform()->R.GetAngle() == rotation ) 
		return NULL;

	
	GTween* old_tween = GetGTween( (void*)e, "entity_rotation" );
	if( old_tween && old_tween->GetUserData().x == rotation ) 
		return NULL;

	// we have old but it's going somewhere else, so let's kill it
	if( old_tween )
		old_tween->Kill();

	GTween* new_tween = new GTween( time, autokill );

		// to identify this
	new_tween->SetName( "entity_rotation" );
	new_tween->SetUserData( types::vector2( rotation, rotation ) );
	new_tween->AddPointer( (void*)e );

	new_tween->SetFunction( math_func );
	
	ceng::IInterpolator* in = ceng::CreateInterpolatorForAngles( 
			ceng::CFunctionPtr<>( &(e->GetTransform()->R), &ceng::math::CMat22< float >::GetAngle ), 
			ceng::CFunctionPtr<>( &(e->GetTransform()->R), &ceng::math::CMat22< float >::SetAngle ), 
			rotation, NULL );

	cassert( in );
	if( in ) 
		in->SetName( "rotation" );

	new_tween->AddInterpolator( in );

	return new_tween;
}





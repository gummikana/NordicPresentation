#ifndef INC_GAMETYPES_H
#define INC_GAMETYPES_H

#include <poro/poro_types.h>
#include <utils/math/cvector2.h>


namespace types {
	struct aabb
	{
		aabb() { }
		aabb( const types::vector2& mini, const types::vector2& maxi ) : mini( mini ), maxi( maxi ) { }

		vector2 mini;
		vector2 maxi;

		
		void ExpandAABB( const types::vector2& p ) 
		{
			ExpandAABB( p.x, p.y );
		}

		void ExpandAABB( float x, float y )
		{
			if( x < mini.x ) 
				mini.x = x;

			if( y < mini.y ) 
				mini.y = y;

			if( x > maxi.x ) 
				maxi.x = x;

			if( y > maxi.y ) 
				maxi.y = y;
		}

	};

	// HAX HAX HAX
	// if you need types::iaabb (int aabb) copy it from cgridworld_thread_impl.cpp into here
}




#endif

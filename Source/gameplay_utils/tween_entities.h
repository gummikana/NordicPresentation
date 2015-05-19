#ifndef INC_TWEEN_ENTITIES_H
#define INC_TWEEN_ENTITIES_H

#include <string>
#include <utils/math/cvector2.h>

namespace ceng { namespace easing { struct IEasingFunc; } }
namespace SGF { class Entity; }
class GTween;

void		GTweenClearEntityOfTweens( SGF::Entity* e );
GTween*		GetGTween( void* uses_pointer, const std::string& name );


GTween* GTweenEntityTo( SGF::Entity* e, const types::vector2& pos, float time, ceng::easing::IEasingFunc& math_func, bool autokill = true );
GTween* GTweenEntityScaleTo( SGF::Entity* e, const types::vector2& scale, float time, ceng::easing::IEasingFunc& math_func, bool autokill = true );
GTween* GTweenEntityRotationTo( SGF::Entity* e, float rotation, float time, ceng::easing::IEasingFunc& math_func, bool autokill = true );



#endif

#ifndef INC_MOUSE_CLICKER_H
#define INC_MOUSE_CLICKER_H

#include <vector>
#include <memory>
#include <poro/default_application.h>

class DebugLayer;
namespace as { class Sprite; class TextSprite; }

class MouseClicker : public poro::DefaultApplication
{
public:
	MouseClicker();

	void Init();
	void Exit();

	void Clear();
	
	//-------

	void Update( float dt );
	void Draw( poro::IGraphics* graphics );

	// -----

	virtual void MouseButtonDown(const poro::types::vec2& pos, int button);
	virtual void MouseButtonUp(const poro::types::vec2& pos, int button);
	virtual void MouseMove(const poro::types::vec2& p);

	virtual void OnKeyDown( int key, poro::types::charset unicode );
	virtual void OnKeyUp( int key, poro::types::charset unicode );

	// -----------

	void SetScore( int score );

	as::Sprite*	mSpriteContainer;
	as::TextSprite* mScore;
	as::Sprite*	mSlider;
	as::Sprite* mBackgroundSprite;
	std::auto_ptr< DebugLayer >		mDebugLayer;

};

#endif

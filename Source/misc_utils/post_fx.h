#ifndef INC_POST_FX_H
#define INC_POST_FX_H

#include <vector>
#include <utils/math/cvector2.h>
#include <utils/hotloader/chotloader.h>


namespace poro { class IGraphics; class ITexture; class ITexture3d; class IShader; class IRenderTexture; }


class PostFx :
	public ceng::IHotloaderListener
{
public:
	PostFx();

	//-------------------------------------------------------------------------
	
	void Init();

	//-------------------------------------------------------------------------

	void Update( float dt, float current_time );

	//-------------------------------------------------------------------------

	void SetGlowTexture( poro::ITexture* texture);

	//-------------------------------------------------------------------------

	void Begin();

	//-------------------------------------------------------------------------

	void End( types::vector2 camera_pos );

	//-------------------------------------------------------------------------

	virtual void OnHotFileChange( const std::string& file );

private:
	void LoadShader( poro::IShader* shader, const std::string& file_vert, const std::string& file_frag );

	struct ColorLookUpData
	{
		ColorLookUpData() : 
			texture( NULL ),
			start_time( 0 )
		{ }

		poro::ITexture3d* texture;
		float start_time;
		float end_time;
	};

	//-------------------------------------------------------------------------

	std::vector< ColorLookUpData* >	mColorLookUps;
	float mTime;
	
	std::auto_ptr< ceng::CHotloader >	mHotLoader;

	// post fx
	bool					mEnabled;
	poro::IRenderTexture*	mDrawBuffer;
	poro::IRenderTexture*	mDrawBufferGlow;
	poro::IRenderTexture*	mDrawBufferGlow2;
	poro::IShader*			mShader;
	poro::IShader*			mShaderGlowX;
	poro::IShader*			mShaderGlowY;

	poro::ITexture3d*		mColorLookupTexturePrev;
	poro::ITexture3d*		mColorLookupTextureNext;
	float					mColorLookupTextureInterpolation;

	poro::ITexture*			mGlowTexture;

	// fog of war test
	poro::ITexture*			mFogOfWarTexture;
};

#endif

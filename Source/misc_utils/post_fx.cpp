#include "post_fx.h"

#include <SDL.h>

#include <map>
#include <math.h>
#include <poro/poro.h>
#include <utils/color/ccolor.h>
#include <utils/array2d/carray2d.h>
#include <utils/imagetoarray/imagetoarray.h>
#include <game_utils/actionscript/sprite.h>
#include "../misc_utils/simple_profiler.h"

#include "../grid/gridworld.h"
#include "../game_globals.h"
#include "../god_game/game_world.h"
#include "../god_game/fog_of_war.h"
#include "../weather/parallax_background.h"

#ifndef FALLING_DONT_INCLUDE_EXTRAS
#include "../path_finding/path_finding.h"
#endif

using namespace poro::types;
using namespace imagetoarray;

namespace
{
	typedef unsigned int Uint32;

	unsigned int GetClut1dIndex(Uint32 size, Uint32 r, Uint32 g, Uint32 b)
	{
        return ((b * size) + g) * size + r;
	}

	ceng::CColorUint8 GetNearest3dCoordInClut( Uint32 size, ceng::CColorUint8 color )
	{
		int div = 256 / size;

		ceng::CColorUint8 result;
		result.r = (ceng::CColorUint8::uint8)( ceng::math::Round( (double)( color.r ) / div ) );
		result.g = (ceng::CColorUint8::uint8)( ceng::math::Round( (double)( color.g ) / div ) );
		result.b = (ceng::CColorUint8::uint8)( ceng::math::Round( (double)( color.b ) / div ) );
		result.a = 0;

		return result;
	}

	void GenerateCLUTFromArviFormat( TempTexture* identity_palette, ceng::CSafeArray< bool, long > defined_color_loopup, Uint32 size, std::string palette_filename )
	{
		int size_image = (int)sqrt( (double)size * size * size );

		TempTexture* palette = GetTexture( palette_filename );
		ceng::CSafeArray< Uint32, long > generated_clut( size * size * size );
		std::map< Uint32, poro::types::vec3 > palette_hash;

		for ( Uint32 r = 0; r < size; r++ )
		{
			for ( Uint32 g = 0; g < size; g++ )
			{
				for ( Uint32 b = 0; b < size; b++ )
				{
					unsigned int index = GetClut1dIndex( size, r, g, b );

					generated_clut[ index ] = ceng::CColorUint8( r * 4, g * 4, b * 4 ).Get32();

				}
			}
		}

		// set the defined colors in the clut
		if( identity_palette && palette 
			&& ( palette->w == identity_palette->w ) 
			&& ( palette->h = identity_palette->h ))
		{
			for( int y = 0; y < palette->h; ++y )
			{
				for( int x = 0; x < palette->w; ++x )
				{
					Uint32 identity_color_32 = GetPixel( identity_palette, x, y, true );
					ceng::CColorUint8 identity_color( identity_color_32 );
					identity_color = GetNearest3dCoordInClut( size, identity_color );

					Uint32 index = GetClut1dIndex( size, identity_color.b, identity_color.g, identity_color.r );
					Uint32 palette_color = GetPixel( palette, x, y, true );

					// swap red and blue
					Uint32 red = (0x00ff0000 & palette_color) >> 16;
					Uint32 blue = (0x000000ff & palette_color) << 16;

					palette_color = ( palette_color & 0xff00ff00 ) | red | blue;
					generated_clut[ index ] = palette_color;
					
					palette_hash.insert( std::pair< Uint32, poro::types::vec3 >( index, poro::types::vec3(identity_color.b, identity_color.g, identity_color.r) ) );
				}
			}
		}

		// generate interpolated colors
		for (Uint32 i = 0; i < size; i++)
		{
			for (Uint32 j = 0; j < size; j++)
			{
				for (Uint32 k = 0; k < size; k++)
				{
					Uint32 index = GetClut1dIndex( size, k, j, i );
					bool is_defined_color = defined_color_loopup[ index ];

					if (is_defined_color == false)
					{
						double r_accum = 0.0;
						double g_accum = 0.0;
						double b_accum = 0.0;
						double a_accum = 0.0;
						double weight_sum = 0.0;

						double nearest_dist = std::numeric_limits<double>::max();

						// interpolate the color from other colors
						for ( std::map< Uint32, poro::types::vec3 >::iterator iter = palette_hash.begin(); iter != palette_hash.end(); ++iter )
						{
							Uint32 index_1d = iter->first;

							if (index_1d != index)
							{
								Uint32 index_1d = iter->first;
								int x2 = index_1d / ( size * size );
								int y2 = (index_1d / size) % size;	
								int z2 = index_1d % size;

								double dist_x = x2 - i;
								double dist_y = y2 - j;
								double dist_z = z2 - k;

								double dist = sqrt( dist_x * dist_x + dist_y * dist_y + dist_z * dist_z );

								if (dist < nearest_dist)
									nearest_dist = dist;
							}
						}
						
						// interpolate the color from other colors
						for ( std::map< Uint32, poro::types::vec3 >::iterator iter = palette_hash.begin(); iter != palette_hash.end(); ++iter )
						{
							Uint32 index_1d = iter->first;
							int x2 = index_1d / ( size * size );
							int y2 = (index_1d / size) % size;	
							int z2 = index_1d % size;

							double dist_x = x2 - i;
							double dist_y = y2 - j;
							double dist_z = z2 - k;

							double dist = sqrt( dist_x * dist_x + dist_y * dist_y + dist_z * dist_z );

							double weight = 1 / dist;
							
							if (dist == nearest_dist)
								weight = 1;
							else
								weight = 0;

							ceng::CColorUint8 color = ceng::CColorUint8( generated_clut[ index_1d ] );
							r_accum += color.r * weight;
							g_accum += color.g * weight;
							b_accum += color.b * weight;

							weight_sum += weight;
						}

						generated_clut[ index ] = ceng::CColorUint8( 
							(ceng::CColorUint8::uint8)( r_accum / weight_sum ),
							(ceng::CColorUint8::uint8)( g_accum / weight_sum),
							(ceng::CColorUint8::uint8)( b_accum / weight_sum) ).Get32();
					}

				}
			}
		}

		ceng::CArray2D< Uint32 > image( size_image, size_image );
		for( int y = 0; y < size_image; ++y )
		{
			for( int x = 0; x < size_image; ++x )
			{
				image.Set( x, y, generated_clut[ y * size_image + x ] );
			}
		}
		SaveImage( "data/weather_gfx/palette_2_clut.png", image );

		delete palette;
	}

	void GenerateCLUTFromArviFormat( std::string identity_palette_filename, std::string palette_filename, Uint32 size = 64 )
	{
		ceng::CSafeArray< bool, long > defined_color_lookup( size * size * size );
		for (Uint32 i=0; i< size * size * size; i++)
			defined_color_lookup[i] = 0;

		TempTexture* identity_palette = GetTexture( identity_palette_filename );

		for( int y = 0; y < identity_palette->h; ++y )
		{
			for( int x = 0; x < identity_palette->w; ++x )
			{
				Uint32 identity_color_32 = GetPixel( identity_palette, x, y, true );
				ceng::CColorUint8 identity_color( identity_color_32 );
				identity_color = GetNearest3dCoordInClut( size, identity_color );
				unsigned int index = GetClut1dIndex( size, identity_color.b, identity_color.g, identity_color.r );
				defined_color_lookup[ index ] = true;
			}
		}

		GenerateCLUTFromArviFormat( identity_palette, defined_color_lookup, size, palette_filename );

		delete identity_palette;
	}

} // end of anonymous namespace


PostFx::PostFx() :
	ceng::IHotloaderListener(),
	mHotLoader( NULL ),
	mTime( 0.f ),
	mEnabled( true ),
	mDrawBuffer( NULL ),
	mDrawBufferGlow( NULL ),
	mDrawBufferGlow2( NULL ),
	mShader( NULL ),
	mShaderGlowX( NULL ),
	mShaderGlowY( NULL ),
	mGlowTexture( NULL)
{
	Init();
}

//-------------------------------------------------------------------------

void PostFx::Init()
{
	// shader hotload
	mHotLoader.reset( new ceng::CHotloader );
	mHotLoader->AddListener( this );
	mHotLoader->SetCheckFilesEveryTSeconds( 0.030f );

	// fog of war
	mFogOfWarTexture = Poro()->GetGraphics()->LoadTexture( "data/temp/fog_of_war_test0.png" );
	//Poro()->GetGraphics()->SetTextureSmoothFiltering( mFogOfWarTexture, true );

	// -- render to texture and post fx ---
	if ( mDrawBuffer != NULL )
		delete mDrawBuffer;

	mDrawBuffer = Poro()->GetGraphics()->CreateRenderTexture( poro::AppConfig().window_w, poro::AppConfig().window_h, true );
	mDrawBufferGlow = Poro()->GetGraphics()->CreateRenderTexture( poro::AppConfig().window_w / 2, poro::AppConfig().window_h / 2, true );
	mDrawBufferGlow2 = Poro()->GetGraphics()->CreateRenderTexture( poro::AppConfig().window_w / 4, poro::AppConfig().window_h / 4, true );

	if ( mShader == NULL )
		mShader = Poro()->GetGraphics()->CreateShader();

	if ( mShaderGlowX == NULL )
		mShaderGlowX = Poro()->GetGraphics()->CreateShader();

	if ( mShaderGlowY == NULL )
		mShaderGlowY = Poro()->GetGraphics()->CreateShader();

	LoadShader( mShader, "data/shaders/default.vert", "data/shaders/daylight_and_weather.frag" );
	LoadShader( mShaderGlowX, "data/shaders/default.vert", "data/shaders/glow_x.frag" );
	LoadShader( mShaderGlowY, "data/shaders/default.vert", "data/shaders/glow_y.frag" );

	//Load color lookup data
	//GenerateCLUTFromArviFormat( "data/weather_gfx/palette_1.png", "data/weather_gfx/palette_1.png" );

	mColorLookUps.resize( 4 );

	mColorLookUps[ 0 ] = new ColorLookUpData;
	mColorLookUps[ 0 ]->texture =  Poro()->GetGraphics()->LoadTexture3d( "data/weather_gfx/clut_day.png" );
	mColorLookUps[ 0 ]->start_time = 0.0f;
	mColorLookUps[ 0 ]->end_time = 0.35f;
	
	mColorLookUps[ 1 ] = new ColorLookUpData;
	mColorLookUps[ 1 ]->texture =  Poro()->GetGraphics()->LoadTexture3d( "data/weather_gfx/clut_dusk.png" );
	mColorLookUps[ 1 ]->start_time = 0.35f;
	mColorLookUps[ 1 ]->end_time = 0.6f;
	
	mColorLookUps[ 2 ] = new ColorLookUpData;
	mColorLookUps[ 2 ]->texture =  Poro()->GetGraphics()->LoadTexture3d( "data/weather_gfx/clut_night.png" );
	mColorLookUps[ 2 ]->start_time = 0.6f;
	mColorLookUps[ 2 ]->end_time = 0.85f;
	
	mColorLookUps[ 3 ] = new ColorLookUpData;
	mColorLookUps[ 3 ]->texture =  Poro()->GetGraphics()->LoadTexture3d( "data/weather_gfx/clut_dawn.png" );
	mColorLookUps[ 3 ]->start_time = 0.85f;
	mColorLookUps[ 3 ]->end_time = 1.0f;
}

//-------------------------------------------------------------------------

void PostFx::Update( float dt, float current_time )
{
	if( mHotLoader.get() ) 
		mHotLoader->Update( dt );

	mTime = current_time;

	if( Poro()->GetKeyboard()->IsKeyJustDown(SDLK_o) && !Poro()->GetKeyboard()->IsCtrlDown()) 
		mEnabled = !mEnabled;

	if ( mEnabled )
	{
		// Calculate the used textures
		ColorLookUpData* lookup;
		int i = 0;
		while ( true )
		{
			lookup = mColorLookUps[ i ];
		
			if ( lookup->end_time > mTime )
				break;

			i++;
		}

		mColorLookupTexturePrev = lookup->texture;

		if (++i == mColorLookUps.size())
			i = 0;

		mColorLookupTextureNext = mColorLookUps[ i ]->texture;
		mColorLookupTextureInterpolation = ( mTime - lookup->start_time ) / ( lookup->end_time - lookup->start_time );
		mColorLookupTextureInterpolation = std::min( std::max( mColorLookupTextureInterpolation, 0.0f  ), 1.0f );
		mColorLookupTextureInterpolation = ( 1.0f - cosf( mColorLookupTextureInterpolation * ceng::math::pi ) ) * 0.5f;
	}
}

//-------------------------------------------------------------------------

void PostFx::SetGlowTexture( poro::ITexture* texture )
{
	mGlowTexture = texture;
}

//-------------------------------------------------------------------------

void PostFx::Begin()
{
	// render to texture proof of concept
	if ( mDrawBuffer && mEnabled )
		mDrawBuffer->BeginRendering();
}

//-------------------------------------------------------------------------

void PostFx::End( types::vector2 camera_pos )
{
	SPROFILE("PostFx");

	if ( mDrawBuffer && mEnabled && mGlowTexture)
	{
		mDrawBuffer->EndRendering();
		
		float scale = GG.GetGameWorld()->GetSpriteContainer()->GetScaleX() * 0.25f; //Magic number? - Every pixel is scaled to 4 times its size...
		int w = poro::AppConfig().window_w;
		int h = poro::AppConfig().window_h;


		// render glow - pass 1
		mDrawBufferGlow->BeginRendering();

		poro::types::vec2 coord1;
		poro::types::vec2 coord2;
		mDrawBufferGlow->GetTexture()->GetUVCoords( coord1, coord2 );
		float offset = (1.0f - coord2.y) * 0.5f;
 
		poro::types::vec2 tex_coords[4];
		tex_coords[3].x = coord2.x;
		tex_coords[3].y = offset;
		tex_coords[2].x = coord2.x;
		tex_coords[2].y = 1.0f - offset;
		tex_coords[1].x = 0;
		tex_coords[1].y = offset;
		tex_coords[0].x = 0;
		tex_coords[0].y = 1.0f - offset;

		poro::types::vec2* tex_coords_ptr = tex_coords;

		mShaderGlowX->Enable();
		mShaderGlowX->SetParameter( "tex",  mGlowTexture );
		Poro()->GetGraphics()->DrawTexturedRect( poro::types::vec2(0, 0), poro::types::vec2( w * scale * 0.5f, h * scale * 0.5f), NULL, poro::GetFColor(1, 1, 1, 1), tex_coords_ptr, 4 );
		mShaderGlowX->Disable();

		mDrawBufferGlow->EndRendering();

		
		// render glow - pass 2
		mDrawBufferGlow2->BeginRendering();

		mDrawBufferGlow2->GetTexture()->GetUVCoords( coord1, coord2 );
		offset = (1.0f - coord2.y) * 0.5f;
 
		tex_coords[3].x = coord2.x;
		tex_coords[3].y = offset;
		tex_coords[2].x = coord2.x;
		tex_coords[2].y = 1.0f - offset;
		tex_coords[1].x = 0;
		tex_coords[1].y = offset;
		tex_coords[0].x = 0;
		tex_coords[0].y = 1.0f - offset;

		tex_coords_ptr = tex_coords;

		mShaderGlowY->Enable();
		mShaderGlowY->SetParameter( "tex",  mDrawBufferGlow->GetTexture() );
		Poro()->GetGraphics()->DrawTexturedRect( poro::types::vec2(0, 0), poro::types::vec2( w * scale * 0.5f, h * scale * 0.5f), NULL, poro::GetFColor(1, 1, 1, 1), tex_coords_ptr, 4 );
		mShaderGlowY->Disable();

		mDrawBufferGlow2->EndRendering();

		// setup fog of war
		GG.GetFogOfWar()->UpdateFogTexture( camera_pos );

		// update shader parameters
		mShader->Enable();

		// default parameters
		mShader->SetParameter( "tex", mDrawBuffer->GetTexture() );
		mShader->SetParameter( "tex_clut_next", mColorLookupTextureNext );
		mShader->SetParameter( "tex_clut_prev", mColorLookupTexturePrev );
		mShader->SetParameter( "tex_glow", mDrawBufferGlow2->GetTexture() );
		mShader->SetParameter( "tex_glow_unfiltered",  mGlowTexture );
		mShader->SetParameter( "tex_fog_of_war", GG.GetFogOfWar()->GetFogTexture() ); // mFogOfWarTexture ); // 
		mShader->SetParameter( "time", Poro()->GetFrameNum() * 0.0166666f );
		mShader->SetParameter( "camera_pos", poro::types::vec2( camera_pos.x, camera_pos.y ) );
		mShader->SetParameter( "clut_interpolation", mColorLookupTextureInterpolation );
		mShader->SetParameter( "storm_interpolation", GG.GetParallaxBackground()->GetStorm() );

		// path finding visualization parameters
#ifndef FALLING_DONT_INCLUDE_EXTRAS

		GG.GetGridWorld()->GetPathFinding()->UpdateVisualizationTexture( camera_pos );
		mShader->SetParameter( "tex_debug", GG.GetGridWorld()->GetPathFinding()->GetVisualizationTexture() );

#endif
		// render
		Poro()->GetGraphics()->DrawTexture( mDrawBuffer->GetTexture(), 0, h * scale, w * scale, -h * scale );

		mShader->Disable();
	}
}

//-------------------------------------------------------------------------

void PostFx::LoadShader( poro::IShader* shader, const std::string& file_vert, const std::string& file_frag )
{
	shader->Init( file_vert, file_frag );

	if( mHotLoader.get() )
	{
		mHotLoader->AddHotFile( file_vert, this );
		mHotLoader->AddHotFile( file_frag, this );
	}
}

//-------------------------------------------------------------------------

void PostFx::OnHotFileChange( const std::string& file )
{
	// just reload all the shaders
	mShader->Init( "data/shaders/default.vert", "data/shaders/daylight_and_weather.frag" );
	mShaderGlowX->Init( "data/shaders/default.vert", "data/shaders/glow_x.frag" );
	mShaderGlowY->Init( "data/shaders/default.vert", "data/shaders/glow_y.frag" );
}

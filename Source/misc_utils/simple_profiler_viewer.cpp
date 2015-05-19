#include "simple_profiler_viewer.h"

#include <game_utils/actionscript/actionscript.h>
#include <utils/color/ccolor.h>

std::string SimpleProfilerViewer::mBuildName = "Build " __DATE__ " " __TIME__;

namespace {
	const std::string font_file = "data/fonts/font_arial.xml";

	struct SimpleProfilerViewerEntrySorter
	{
		inline bool operator() (const SimpleProfilerViewerEntry* b, const SimpleProfilerViewerEntry* a)
		{
			return a->mRollingAverage < b->mRollingAverage;
		}
	};
}

//-----------------------------------------------------------------------------

SimpleProfilerViewer::SimpleProfilerViewer() : 
	as::Sprite(),
	mLabels( NULL ),
	mEntries()
{
	mLabels = new SimpleProfilerViewerEntry;
	mLabels->DoLabels();
	addChild( mLabels );

	mBuildNameSprite = as::LoadTextSprite( font_file );
	mBuildNameSprite->SetColor( 1, 0, 0 );
	mBuildNameSprite->SetText( mBuildName );
	float x_position = 1024 - ( mBuildNameSprite->GetSize().x + 10.f);
	mBuildNameSprite->MoveTo( types::vector2( x_position, 768 - 20.f ) );

	addChild( mBuildNameSprite );

	Update(0.0f);
	SortEntries();
}
	
void SimpleProfilerViewer::Update( float dt )
{
	as::Sprite::Update( dt );

	SimpleProfilerGlobal* global = ceng::GetSingletonPtr< SimpleProfilerGlobal >();
	for( std::map< std::string, SimpleProfilerGlobal::TProfilerData* >::const_iterator i = global->mDataMap.begin(); i != global->mDataMap.end(); ++i )
	{
		SimpleProfilerViewerEntry* entry = GetEntry( i->first );
		if( entry ) entry->ParseStats( i->first, i->second );
	}
}

SimpleProfilerViewerEntry* SimpleProfilerViewer::GetEntry( const std::string& name )
{
	if( mEntries[ name ] == NULL ) 
	{
		SimpleProfilerViewerEntry* entry = new SimpleProfilerViewerEntry;
		entry->MoveTo( types::vector2( 0, ( mEntries.size() ) * 20.f ) );
		mEntries[ name ] = entry;
		addChild( entry );
	}

	return mEntries[ name ];
}

void SimpleProfilerViewer::SortEntries()
{
	std::vector< SimpleProfilerViewerEntry* > entries_sorted;
	for ( std::map< std::string, SimpleProfilerViewerEntry* >::iterator it = mEntries.begin(); it != mEntries.end(); ++it)
		entries_sorted.push_back(it->second);

	std::sort( entries_sorted.begin(), entries_sorted.end(), SimpleProfilerViewerEntrySorter() );

	for (size_t i = 0; i < entries_sorted.size(); ++i)
		entries_sorted[i]->MoveTo(types::vector2(0, (i + 1) * 20.f));
}
	
//-----------------------------------------------------------------------------

SimpleProfilerViewerEntry::SimpleProfilerViewerEntry() :
	mTextSprites()
{
	mTextSprites.resize( _SPRITE_COUNT );
	const float task_name_length = 260.f;
	float x_step = ( 900.f - task_name_length ) / ( _SPRITE_COUNT );
	for( int i = 0; i < _SPRITE_COUNT; ++i ) 
	{
		as::TextSprite* text_sprite = as::LoadTextSprite( font_file );
		text_sprite->SetColor( 1, 1, 1 );
		float x_position = task_name_length + (i - 1) * x_step;
		if( i == 0 ) x_position = 10.f;
		text_sprite->MoveTo( types::vector2( x_position, 0 ) );

		addChild( text_sprite );
		mTextSprites[ i ] = text_sprite;
	}
	
}

namespace {
	
	template< class T >
	std::string Profiler_ToString( T value, int precision = 4 )
	{
		if( value == 0 ) return "0";
		std::stringstream ss;
		ss << std::fixed << std::setprecision( precision );
		ss << value;
		return ss.str();
	}

	void SetColorToSprite( as::Sprite* sprite, poro::types::Uint32 color )
	{
		poro::types::Uint32 r32, g32, b32;
		
		r32 = color & types::fcolor::RMask;
		g32 = color & types::fcolor::GMask;
		b32 = color & types::fcolor::BMask;

		float r = ( r32 >> types::fcolor::RShift ) / 255.f;
		float g = ( g32 >> types::fcolor::GShift ) / 255.f;
		float b = ( b32 >> types::fcolor::BShift ) / 255.f;

		sprite->SetColor( b, g, r );
	}
} // end of anonymous namespace

void SimpleProfilerViewerEntry::ParseStats( const std::string& name, const SimpleProfilerGlobal::TProfilerData* data )
{
	SPROFILE("ParseStats");

	if( data->mColor != 0 ) 
	{
		for( int i = 0; i < _SPRITE_COUNT; ++i )
			SetColorToSprite( mTextSprites[i], data->mColor );
	}

	double ms_per_frame = 1.0 / Poro()->GetFrameRate() * 1000;
	double percent_of_frame = data->GetCurrent() / ms_per_frame * 100.0;

	mRollingAverage = data->GetRollingAverage();

	mTextSprites[ SPRITE_NAME		]->SetText( name );
	mTextSprites[ SPRITE_CURRENT	]->SetText(		Profiler_ToString( data->GetCurrent() ) );
	mTextSprites[ SPRITE_AVERAGE	]->SetText(		Profiler_ToString( data->GetAverage() ) );
	// mTextSprites[ SPRITE_MIN_VALUE	]->SetText(		Profiler_ToString( data->GetMin() ) );
	mTextSprites[ SPRITE_MAX_VALUE	]->SetText(		Profiler_ToString( data->GetMax() ) );
	mTextSprites[ SPRITE_TOTAL_VALUE ]->SetText(	Profiler_ToString( data->GetTotal() ) );
	mTextSprites[ SPRITE_PERCENT_OF_FRAME]->SetText(Profiler_ToString(percent_of_frame, 2) );
	mTextSprites[ SPRITE_ROLLING_AVERAGE ]->SetText( Profiler_ToString( mRollingAverage ) );
}

void SimpleProfilerViewerEntry::DoLabels()
{
	mTextSprites[ SPRITE_NAME		]->SetText( "NAME" );
	mTextSprites[ SPRITE_CURRENT	]->SetText( "LATEST" );
	mTextSprites[ SPRITE_AVERAGE	]->SetText(	"AVERAGE" );
	// mTextSprites[ SPRITE_MIN_VALUE	]->SetText(	"MINIMUM" );
	mTextSprites[ SPRITE_MAX_VALUE	]->SetText(	"MAXIMUM" );
	mTextSprites[ SPRITE_TOTAL_VALUE ]->SetText( "TOTAL TIME" );
	mTextSprites[ SPRITE_PERCENT_OF_FRAME]->SetText("FRAME %");
	mTextSprites[ SPRITE_ROLLING_AVERAGE ]->SetText( "ROLLING AVERAGE" );
}

//------------------------------------------------------------------------------------------

void SimpleProfilerWriteToFile( const std::string& filename, bool to_std_cout )
{
	std::fstream foutput( filename.c_str(), std::ios::out );

	SimpleProfilerGlobal* global = ceng::GetSingletonPtr< SimpleProfilerGlobal >();
	for( std::map< std::string, SimpleProfilerGlobal::TProfilerData* >::const_iterator i = global->mDataMap.begin(); i != global->mDataMap.end(); ++i )
	{
		std::string name = i->first;
		const SimpleProfilerGlobal::TProfilerData* data = i->second;

		foutput << name << ": average " << data->GetAverage()  << ", max " << data->GetMax() << ", total " << data->GetTotal() << std::endl;
		if( to_std_cout ) 
			std::cout << name << ": average " << data->GetAverage()  << ", max " << data->GetMax() << ", total " << data->GetTotal() << std::endl;
	}

	foutput.close();
}

//------------------------------------------------------------------------------------------

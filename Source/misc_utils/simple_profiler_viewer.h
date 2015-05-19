#ifndef INC_SIMPLE_PROFILER_VIEWER_H
#define INC_SIMPLE_PROFILER_VIEWER_H

#include <map>
#include <string>

#include <game_utils/actionscript/sprite.h>
#include "simple_profiler.h"

class SimpleProfilerViewerEntry;
namespace as { class TextSprite; }

#define SPROFILE_SETBUILDNAME( build_name ) ::SimpleProfilerViewer::mBuildName = build_name

//-----------------------------------------------------------------------------

class SimpleProfilerViewer : public as::Sprite
{
public:
	
	SimpleProfilerViewer();
	
	virtual void				Update( float dt );
	SimpleProfilerViewerEntry*	GetEntry( const std::string& name );
	void						SortEntries();

	
	SimpleProfilerViewerEntry* mLabels;
	std::map< std::string, SimpleProfilerViewerEntry* > mEntries;
	as::TextSprite* mBuildNameSprite;

	static std::string mBuildName;
};

//-----------------------------------------------------------------------------

class SimpleProfilerViewerEntry : public as::Sprite
{
public:
	enum {
		SPRITE_NAME,
		SPRITE_CURRENT,
		SPRITE_AVERAGE,
		// SPRITE_MIN_VALUE,
		SPRITE_MAX_VALUE,
		SPRITE_TOTAL_VALUE,
		SPRITE_PERCENT_OF_FRAME,
		SPRITE_ROLLING_AVERAGE,
		_SPRITE_COUNT 
	};

	SimpleProfilerViewerEntry();


	void ParseStats( const std::string& name, const SimpleProfilerGlobal::TProfilerData* data );
	void DoLabels();


	double mRollingAverage;
	std::vector< as::TextSprite* > mTextSprites;
};

//-----------------------------------------------------------------------------

#endif

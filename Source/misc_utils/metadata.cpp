#include "metadata.h"

//-----------------------------------------------------------------------------

const Interface_MetaData& MetaData( int variable )
{
	static Interface_MetaData mMetaData;
	mMetaData.Init();
	if( variable == NULL ) {
		mMetaData.type = Interface_MetaData::TYPE_NULL;
	}
	return mMetaData;
}

const Interface_MetaData& MetaData( int min_v, int max_v )
{
	static Interface_MetaData mMetaData;
	mMetaData.Init();
	mMetaData.type = Interface_MetaData::TYPE_MIN_MAX;
	mMetaData.min_value = (double)min_v;
	mMetaData.max_value = (double)max_v;

	return mMetaData;
}

const Interface_MetaData& MetaData( float min_v, float max_v )
{
	static Interface_MetaData mMetaData;
	mMetaData.Init();
	mMetaData.type = Interface_MetaData::TYPE_MIN_MAX;
	mMetaData.min_value = (double)min_v;
	mMetaData.max_value = (double)max_v;
	return mMetaData;
}

const Interface_MetaData& MetaData( double min_v, double max_v )
{
	static Interface_MetaData mMetaData;
	mMetaData.Init();
	mMetaData.type = Interface_MetaData::TYPE_MIN_MAX;
	mMetaData.min_value = (double)min_v;
	mMetaData.max_value = (double)max_v;
	return mMetaData;
}

const Interface_MetaData& MetaFile( const std::string& path, const std::string& extension  )
{
	static Interface_MetaData mMetaData;
	mMetaData.Init();
	mMetaData.type = Interface_MetaData::TYPE_FILE;
	mMetaData.path = path;
	mMetaData.extension = extension;
	return mMetaData;
}

const IMetaComment& MetaComment( const std::string& comment )
{
	static IMetaComment mMetaData;
	mMetaData.Init();
	mMetaData.type = Interface_MetaData::TYPE_COMMENT;
	mMetaData.comment = comment;
	return mMetaData;
}

const Interface_MetaDataNoSerialize&  MetaPrivate()
{
	static Interface_MetaDataNoSerialize mMetaData;
	mMetaData.Init();
	return mMetaData;
}

const IMetaObject& MetaObject()
{
	static IMetaObject mMetaData;
	mMetaData.Init();
	return mMetaData;
}

//-----------------------------------------------------------------------------

const Interface_MetaData& operator+ (const Interface_MetaData& data, const IMetaComment& comment)
{
	static Interface_MetaData mMetaData;
	mMetaData.Init();

	if( (&data) != NULL ) 
	{
		mMetaData.type = data.type;

		// min - max value
		mMetaData.min_value = data.min_value;
		mMetaData.max_value = data.max_value;

		// file
		mMetaData.path = data.path;
		mMetaData.extension = data.extension;
	}
	
	// comment
	if( (&comment) != NULL )
	{
		mMetaData.comment = comment.comment;
	}

	return mMetaData;
}

const Interface_MetaDataNoSerialize& operator+ (const Interface_MetaDataNoSerialize& data, const IMetaComment& comment)
{
	static Interface_MetaDataNoSerialize mMetaData;
	mMetaData.Init();

	if( (&data) != NULL ) 
	{
		mMetaData.type = data.type;

		// min - max value
		mMetaData.min_value = data.min_value;
		mMetaData.max_value = data.max_value;

		// file
		mMetaData.path = data.path;
		mMetaData.extension = data.extension;
	}
	
	// comment
	if( (&comment) != NULL )
	{
		mMetaData.comment = comment.comment;
	}

	return mMetaData;
}

const IMetaObject& operator+ (const IMetaObject& data, const IMetaComment& comment)
{
	static IMetaObject mMetaData;
	mMetaData.Init();

	if( (&data) != NULL ) 
	{
		mMetaData.type = data.type;

		// min - max value
		mMetaData.min_value = data.min_value;
		mMetaData.max_value = data.max_value;

		// file
		mMetaData.path = data.path;
		mMetaData.extension = data.extension;
	}
	
	// comment
	if( (&comment) != NULL )
	{
		mMetaData.comment = comment.comment;
	}

	return mMetaData;
}

//-----------------------------------------------------------------------------

Interface_MetaData::Interface_MetaData() { Init(); }

Interface_MetaData::Interface_MetaData( int variable ) 
{
	Init();
	if( variable == NULL ) {
		type = TYPE_NULL;
	}
}

Interface_MetaData::Interface_MetaData( const std::string& comment ) 
{
	Init();
	// this is a comment meta data
	type = TYPE_COMMENT;
}

Interface_MetaData::Interface_MetaData( float min_v, float max_v ) 
{
	Init();
	// this is a slider min, max data structure
	type = TYPE_MIN_MAX;
	min_value = (double)min_v;
	max_value = (double)max_v;
}

Interface_MetaData::Interface_MetaData( int min_v, int max_v )
{
	Init();
	// this is a slider min, max data structure
	type = TYPE_MIN_MAX;
	min_value = (double)min_v;
	max_value = (double)max_v;
}

Interface_MetaData::Interface_MetaData( double min_v, double max_v )
{
	Init();
	// this is a slider min, max data structure
	type = TYPE_MIN_MAX;
	min_value = (double)min_v;
	max_value = (double)max_v;
}


void Interface_MetaData::Init()
{
	type = TYPE_NULL;
	min_value = 0;
	max_value = 1;
}

//-----------------------------------------------------------------------------
#ifndef INC_METADATA_H
#define INC_METADATA_H

#include <string>

//-----------------------------------------------------------------------------

struct Interface_MetaData;
struct IMetaComment;
struct Interface_MetaDataNoSerialize;
struct IMetaObject;

const Interface_MetaData&			MetaData( int variable );
const Interface_MetaData&			MetaData( int min_value, int max_value );
const Interface_MetaData&			MetaData( float min_value, float max_value );
const Interface_MetaData&			MetaData( double min_value, double max_value );

const Interface_MetaData&			MetaFile( const std::string& path = "data/", const std::string& extension = ""  );

const IMetaComment&			MetaComment( const std::string& comment );

const Interface_MetaDataNoSerialize&	MetaPrivate();
const IMetaObject&			MetaObject();


//-----------------------------------------------------------------------------

struct Interface_MetaData
{
	enum MetaDataSerialize
	{
		META_SERIALIZE = 1,
		META_SERIALIZE_BIND = 0,
		META_CONFIG_SLIDERS = 1
	};

	enum MetaDataTypes
	{
		TYPE_NULL = 0,
		TYPE_COMMENT = 1,
		TYPE_MIN_MAX = 2,
		TYPE_FILE = 3,
	};

	Interface_MetaData();
	Interface_MetaData( int variable );
	Interface_MetaData( const std::string& comment );
	Interface_MetaData( int min_value, int max_value );
	Interface_MetaData( float min_value, float max_value );
	Interface_MetaData( double min_value, double max_value );

	virtual ~Interface_MetaData() { }

	void Init();

	int type;

	// min - max value
	double min_value;
	double max_value;

	// file
	std::string path;
	std::string extension;

	// comment
	std::string comment;

	virtual bool Serialize() const { return true; }
	virtual bool IsMetaObject() const { return false; }
};

//-----------------------------------------------------------------------------

struct IMetaComment : public Interface_MetaData
{
};

//-----------------------------------------------------------------------------

struct Interface_MetaDataNoSerialize : public Interface_MetaData
{
	enum MetaDataSerialize
	{
		META_SERIALIZE = 0,
		META_SERIALIZE_BIND = 0,
		META_CONFIG_SLIDERS = 0
	};

	virtual bool Serialize() const { return false; }
};

//-----------------------------------------------------------------------------

struct IMetaObject : public Interface_MetaData 
{
	enum MetaDataSerialize
	{
		META_SERIALIZE = 0,
		META_SERIALIZE_BIND = 1,
		META_CONFIG_SLIDERS = 0
	};

	IMetaObject() { }

	virtual bool Serialize() const { return true; }
	virtual bool IsMetaObject() const { return true; }
};

//-----------------------------------------------------------------------------

const Interface_MetaData&			operator+ (const Interface_MetaData& data, const IMetaComment& comment);
const Interface_MetaDataNoSerialize& operator+ (const Interface_MetaDataNoSerialize& data, const IMetaComment& comment);
const IMetaObject&			operator+ (const IMetaObject& metaobject, const IMetaComment& comment);

//-----------------------------------------------------------------------------

#endif

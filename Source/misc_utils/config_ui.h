#ifndef INC_CONFIG_UI_H
#define INC_CONFIG_UI_H

/*
#define CONFIG_BIOME(list_) \
	list_(float,			mPerlinScale_x,					1.f,			MetaData( 0.f, 512.f ) ) \
	list_(float,			mPerlinScale_y,					1.f,			MetaData( 0.f, 512.f ) ) \
	list_(int,				mGradientType,					1,				MetaData( 0, 2 ) ) \
	list_(std::string,		mInsidePerlinFunc,				"",				NULL ) \
	list_(bool,				mGradientAddNoise,				1,				NULL ) \

DEFINE_CONFIG_UI( Biome, CONFIG_BIOME );

#undef CONFIG_BIOME
*/

#include <map>
#include <string>
#include <utils/xml/canycontainer.h>

#include "metadata.h"
#include "config_sliders.h"


//-----------------------------------------------------------------------------

namespace ceng {

class IConfigBase
{
public:
	virtual ~IConfigBase() { }

	//---- serialize ----
	virtual void			Serialize( ceng::CXmlFileSys* filesys ) { }
	virtual ConfigSliders*  GetConfigSliders( ConfigSliders* config_sliders = NULL, IConfigSlidersListener* listener = NULL ) = 0;

	//----- property accessors -------
	virtual const std::map< std::string, bool >&	GetListOfVariables() const = 0;
	virtual void									SetValue( const std::string& n, const ceng::CAnyContainer& new_value ) = 0;
	virtual ceng::CAnyContainer						GetValue( const std::string& n ) const = 0;
	virtual Interface_MetaData								GetMetaData( const std::string& n ) const = 0;
	
	virtual IConfigBase*							GetMetaObject( const std::string& member_name ) = 0;

	template< class T >
	T GetValueAs( const std::string& n ) const {
		return ceng::CAnyContainerCast< T >( GetValue( n ) );
	}

};

//-----------------------------------------------------------------------------

template< class T >
struct META_IsPointer
{
	enum { value = 0 };
};

template< class T >
struct META_IsPointer< T* >
{
	enum { value = 1 };
};

template< class T >
bool IsPointer( const T& value ) 
{
	if( META_IsPointer< T >::value == 0 ) return false;
	else return true;
}

template< class T >
T* GetAsPointer( T& value )
{
	return &value;
}

template< class T >
T* GetAsPointer( T* value )
{
	return value;
}

// -- template hack for MetaObjects --
template< class T >
ceng::IConfigBase* TEMPLATE_HACK_GET_AS_META_OBJECT( T& variable, const IMetaObject& meta_object )
{ 
	return GetAsPointer( variable );
}

template< class T >
ceng::IConfigBase* TEMPLATE_HACK_GET_AS_META_OBJECT( T& variable, const Interface_MetaData& meta_data ) {
	return NULL;
}

template< class T >
ceng::IConfigBase* TEMPLATE_HACK_GET_AS_META_OBJECT( T& variable, const int& null_object ) {
	return NULL;
}

// -- Serialize helper --

template< class T > 
bool SerializeThis( const T& value, const int& null_reference )
{
	if( META_IsPointer< T >::value == 1 ) return false;
	return true;
}

template< class T, class MData > 
bool SerializeThis( const T& value, const MData& mt )
{
	if( META_IsPointer< T >::value == 1 ) return false;
	if( mt.Serialize() == false ) return false;
	return true;
}

int SerializeBindMe( const int& mt );

inline int SerializeBindMe( const int& mt )
{
	return 0;
}

template< class MData > 
int SerializeBindMe( const MData& mt )
{
	if( MData::META_SERIALIZE_BIND == 1 ) return 1;
	else return 0;
}

template< class MData >
bool SerializeIsMetaObject( const MData& mt )
{
	return mt.IsMetaObject();
}

bool SerializeIsMetaObject( int mt );

inline bool SerializeIsMetaObject( int mt )
{
	return false;
}

template< class T >
void TEMPLATE_HACK_FOR_XML_Bind( ceng::CXmlFileSys* filesys, T& variable, const std::string& name,  const Interface_MetaDataNoSerialize& meta_data_serialize )
{ }

template< class T >
void TEMPLATE_HACK_FOR_XML_Bind( ceng::CXmlFileSys* filesys, T& variable, const std::string& name,  const IMetaObject& meta_data_serialize ) {
	XML_BindAlias( filesys, variable, name );	
}

template< class T >
void TEMPLATE_HACK_FOR_XML_Bind( ceng::CXmlFileSys* filesys, T& variable, const std::string& name,  const Interface_MetaData& meta_data_serialize ) {
	XML_BindAttributeAlias( filesys, variable, name );
}


template< class T >
void TEMPLATE_HACK_FOR_XML_Bind( ceng::CXmlFileSys* filesys, T& variable, const std::string& name,  const int& null_object ) {
	XML_BindAttributeAlias( filesys, variable, name );
}

#define CONFIG_UI_HEADER_DECLARE(type, name, value, meta_data) \
    type name; 
	
#define CONFIG_UI_GETTER_DECLARE(type, name, value, meta_data) \
	const type& Get_##name() const { return (const type&)name; }

#define CONFIG_UI_XML_BIND_DECLARE(type, name, value, meta_data) \
	if( ceng::SerializeThis( name, meta_data ) ) { \
		TEMPLATE_HACK_FOR_XML_Bind( filesys, name, #name, meta_data ); \
		if( filesys->GetNode()->HasAttribute( #name ) ) _m_variables[ #name ] = true;  \
	}

#define CONFIG_UI_XML_BIND_DECLARE_NO_M_VARIABLES(type, name, value, meta_data) \
	if( ceng::SerializeThis( name, meta_data ) ) { \
		TEMPLATE_HACK_FOR_XML_Bind( filesys, name, #name, meta_data ); \
	}

#define CONFIG_UI_INIT_MEMBERS(type, name, value, meta_data) \
	name = value; \
	_m_variables[ #name ] = true;

#define CONFIG_UI_JUST_INIT_MEMBERS(type, name, value, meta_data) \
	name = value; 

#define CONFIG_UI_SET_OTHER(type, name, value, meta_data) \
	name = other.name; 

#define CONFIG_UI_SET_VALUE(type, name, value, meta_data) \
	if( n == #name ) { name = ceng::CAnyContainerCast< type >( new_value ); return; }

#define CONFIG_UI_GET_VALUE(type, name, value, meta_data) \
	if( n == #name ) return name;

#define CONFIG_UI_GET_META_DATA(type, name, value, meta_data) \
	if( n == #name ) return meta_data;

#define CONFIG_UI_GET_CONFIGSLIDERS(type, name, value, meta_data) \
	AddToConfigSlidersImpl( _temp_configsliders_result, name, #name, meta_data );

#define CONFIG_UI_GET_META_OBJECT(type, name, value, meta_data) \
	if( #name == member_name && ceng::SerializeIsMetaObject( meta_data ) ) return ceng::TEMPLATE_HACK_GET_AS_META_OBJECT( name, meta_data );

// _temp_configsliders_result
//-----------------------------------------------------------------------------

#define DEFINE_CONFIG_UI(name, list)                 \
	class name : public ceng::IConfigBase { \
	public: \
		name() { \
			list(CONFIG_UI_INIT_MEMBERS) \
		} \
		\
		name( const name& other ) { Set( other ); } \
		\
		const name& operator= ( const name& other ) { Set( other ); return *this; } \
		\
		const name& operator+= ( const name& other ) \
		{ \
			for( std::map< std::string, bool >::const_iterator i = other._m_variables.begin(); i != other._m_variables.end(); ++i ) { \
				if( i->second ) SetValue( i->first, other.GetValue( i->first ) ); \
			} \
			return *this; \
		} \
		\
        list(CONFIG_UI_HEADER_DECLARE)   \
		std::map< std::string, bool > _m_variables; \
		\
		const std::map< std::string, bool >& GetListOfVariables() const { return _m_variables; } \
		std::string GetName() const { return #name; } \
		\
		\
		void Serialize( ceng::CXmlFileSys* filesys ) \
		{ \
			list(CONFIG_UI_XML_BIND_DECLARE) \
		} \
		\
		void Set( const name& other ) \
		{ \
			list(CONFIG_UI_SET_OTHER) \
			_m_variables = other._m_variables; \
		} \
		\
		void SetValue( const std::string& n, const ceng::CAnyContainer& new_value ) \
		{ \
			list(CONFIG_UI_SET_VALUE) \
		} \
		template< class T > void AddToConfigSlidersImpl( ConfigSliders* _temp_configsliders_result, T& variable, const std::string& name, const Interface_MetaDataNoSerialize& meta_data ) { } \
		\
		template< class T > void AddToConfigSlidersImpl( ConfigSliders* _temp_configsliders_result, T& variable, const std::string& name, const IMetaObject& meta_data ) \
		{ \
			variable.GetConfigSliders( _temp_configsliders_result, _temp_configsliders_result->GetListener() ); \
		} \
		\
		template< class T > void AddToConfigSlidersImpl( ConfigSliders* _temp_configsliders_result, T& variable, const std::string& name, const Interface_MetaData& meta_data ) \
		{ \
			_temp_configsliders_result->AddVariable(variable, name, meta_data ); \
		} \
		\
		ConfigSliders* GetConfigSliders( IConfigSlidersListener* listener = NULL ) \
		{ \
			ConfigSliders* _temp_configsliders_result = new ConfigSliders( listener ); \
			return GetConfigSliders( _temp_configsliders_result, listener ); \
		} \
		ConfigSliders* GetConfigSliders( ConfigSliders* _temp_configsliders_result = NULL, IConfigSlidersListener* listener = NULL ) \
		{ \
			if( _temp_configsliders_result == NULL ) _temp_configsliders_result = new ConfigSliders( listener ); \
			list(CONFIG_UI_GET_CONFIGSLIDERS) \
			return _temp_configsliders_result; \
		} \
		\
		ceng::CAnyContainer GetValue( const std::string& n ) const \
		{ \
			list(CONFIG_UI_GET_VALUE) \
			return ceng::CAnyContainer(); \
		} \
		\
		Interface_MetaData GetMetaData( const std::string& n ) const \
		{ \
			list(CONFIG_UI_GET_META_DATA) \
			return Interface_MetaData( NULL ); \
		} \
		\
		ceng::IConfigBase* GetMetaObject( const std::string& member_name ) \
		{ \
			void* _temp_configsliders_result = NULL; \
			list(CONFIG_UI_GET_META_OBJECT) \
			return static_cast< ceng::IConfigBase* >( _temp_configsliders_result ); \
		} \
		\
	};


} // end of namespace ceng
//-----------------------------------------------------------------------------

#endif

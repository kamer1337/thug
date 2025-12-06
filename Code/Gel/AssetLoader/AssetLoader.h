/*****************************************************************************
**																			**
**					   	  Neversoft Entertainment							**
**																		   	**
**				   Copyright (C) 1999 - All Rights Reserved				   	**
**																			**
******************************************************************************
**																			**
**	Project:		GEL (Game Engine Library)								**
**																			**
**	Module:			AssetLoader												**
**																			**
**	File name:		AssetLoader.h											**
**																			**
**	Created by:		PC Port													**
**																			**
**	Description:	Unified asset loader for audio, video, and other files	**
**																			**
*****************************************************************************/

#ifndef __GEL_ASSETLOADER_H
#define __GEL_ASSETLOADER_H

/*****************************************************************************
**							  	  Includes									**
*****************************************************************************/

#ifndef __CORE_DEFINES_H
#include <core/defines.h>
#endif

#include <core/hashtable.h>
#include <core/string/cstring.h>

/*****************************************************************************
**								   Defines									**
*****************************************************************************/

namespace Script
{
	class CStruct;
}

namespace Asset
{

/*****************************************************************************
**							     Type Defines								**
*****************************************************************************/

// Asset types supported by the loader
enum EAssetType
{
	ASSET_TYPE_UNKNOWN = 0,
	ASSET_TYPE_AUDIO,
	ASSET_TYPE_VIDEO,
	ASSET_TYPE_TEXTURE,
	ASSET_TYPE_MODEL,
	ASSET_TYPE_ANIMATION,
	ASSET_TYPE_SCRIPT,
};

// Audio format types
enum EAudioFormat
{
	AUDIO_FORMAT_UNKNOWN = 0,
	AUDIO_FORMAT_WAV,
	AUDIO_FORMAT_OGG,
	AUDIO_FORMAT_MP3,
	AUDIO_FORMAT_FLAC,
};

// Video format types
enum EVideoFormat
{
	VIDEO_FORMAT_UNKNOWN = 0,
	VIDEO_FORMAT_MP4,
	VIDEO_FORMAT_AVI,
	VIDEO_FORMAT_MOV,
	VIDEO_FORMAT_WMV,
};

// Asset loading flags
enum EAssetLoadFlags
{
	ASSET_LOAD_ASYNC		= (1 << 0),	// Load asynchronously
	ASSET_LOAD_STREAMING	= (1 << 1),	// Use streaming mode (for large files)
	ASSET_LOAD_PRELOAD		= (1 << 2),	// Preload entire file into memory
	ASSET_LOAD_CACHED		= (1 << 3),	// Keep in cache after use
	ASSET_LOAD_PERMANENT	= (1 << 4),	// Never unload
};

// Asset status
enum EAssetStatus
{
	ASSET_STATUS_UNLOADED = 0,
	ASSET_STATUS_LOADING,
	ASSET_STATUS_LOADED,
	ASSET_STATUS_ERROR,
};

/*****************************************************************************
**							     Structures									**
*****************************************************************************/

// Base asset data structure
struct AssetData
{
	uint32			checksum;			// Unique identifier
	char			filename[256];		// File path
	EAssetType		type;				// Asset type
	EAssetStatus	status;				// Current status
	uint32			flags;				// Loading flags
	void*			data;				// Pointer to loaded data
	size_t			dataSize;			// Size of data in bytes
	uint32			refCount;			// Reference count
	
	AssetData()
	{
		checksum = 0;
		filename[0] = '\0';
		type = ASSET_TYPE_UNKNOWN;
		status = ASSET_STATUS_UNLOADED;
		flags = 0;
		data = NULL;
		dataSize = 0;
		refCount = 0;
	}
};

// Audio asset data
struct AudioAssetData
{
	EAudioFormat	format;				// Audio format
	uint32			sampleRate;			// Sample rate (Hz)
	uint32			channels;			// Number of channels (1=mono, 2=stereo)
	uint32			bitsPerSample;		// Bits per sample
	float			duration;			// Duration in seconds
	bool			is3D;				// Is this a 3D positional sound?
	
	AudioAssetData()
	{
		format = AUDIO_FORMAT_UNKNOWN;
		sampleRate = 0;
		channels = 0;
		bitsPerSample = 0;
		duration = 0.0f;
		is3D = false;
	}
};

// Video asset data
struct VideoAssetData
{
	EVideoFormat	format;				// Video format
	uint32			width;				// Video width
	uint32			height;				// Video height
	float			frameRate;			// Frames per second
	float			duration;			// Duration in seconds
	bool			hasAudio;			// Does video have audio track?
	
	VideoAssetData()
	{
		format = VIDEO_FORMAT_UNKNOWN;
		width = 0;
		height = 0;
		frameRate = 0.0f;
		duration = 0.0f;
		hasAudio = false;
	}
};

/*****************************************************************************
**							     Asset Loader Class							**
*****************************************************************************/

class CAssetLoader : public Spt::Class
{
	DeclareSingletonClass( CAssetLoader );

public:
	CAssetLoader( void );
	~CAssetLoader( void );

	// Initialization
	void					Init( void );
	void					Deinit( void );
	
	// Asset loading/unloading
	AssetData*				LoadAsset( const char* filename, EAssetType type, uint32 flags = 0 );
	AssetData*				LoadAsset( uint32 checksum, EAssetType type, uint32 flags = 0 );
	void					UnloadAsset( AssetData* pAsset );
	void					UnloadAsset( uint32 checksum );
	void					UnloadAllAssets( bool includePermanent = false );
	
	// Asset access
	AssetData*				GetAsset( const char* filename );
	AssetData*				GetAsset( uint32 checksum );
	bool					IsAssetLoaded( const char* filename );
	bool					IsAssetLoaded( uint32 checksum );
	
	// Reference counting
	void					AddRef( AssetData* pAsset );
	void					Release( AssetData* pAsset );
	
	// Audio-specific functions
	AudioAssetData*			LoadAudioAsset( const char* filename, uint32 flags = 0 );
	EAudioFormat			GetAudioFormat( const char* filename );
	
	// Video-specific functions
	VideoAssetData*			LoadVideoAsset( const char* filename, uint32 flags = 0 );
	EVideoFormat			GetVideoFormat( const char* filename );
	
	// Utility functions
	EAssetType				DetectAssetType( const char* filename );
	const char*				GetAssetTypeName( EAssetType type );
	const char*				GetAssetStatusName( EAssetStatus status );
	
	// Statistics
	int						GetNumLoadedAssets( void );
	size_t					GetTotalMemoryUsage( void );
	void					PrintStatistics( void );

private:
	// Internal helper functions
	AssetData*				CreateAssetData( const char* filename, EAssetType type, uint32 flags );
	bool					LoadAudioData( AssetData* pAsset );
	bool					LoadVideoData( AssetData* pAsset );
	void					FreeAssetData( AssetData* pAsset );
	uint32					CalculateChecksum( const char* str );
	const char*				GetFileExtension( const char* filename );
	
	// Asset storage
	Lst::HashTable<AssetData>*	mp_assetTable;
	bool						m_initialized;
};

/*****************************************************************************
**							  Inline Functions								**
*****************************************************************************/

inline const char* CAssetLoader::GetAssetTypeName( EAssetType type )
{
	switch (type)
	{
		case ASSET_TYPE_AUDIO:		return "Audio";
		case ASSET_TYPE_VIDEO:		return "Video";
		case ASSET_TYPE_TEXTURE:	return "Texture";
		case ASSET_TYPE_MODEL:		return "Model";
		case ASSET_TYPE_ANIMATION:	return "Animation";
		case ASSET_TYPE_SCRIPT:		return "Script";
		default:					return "Unknown";
	}
}

inline const char* CAssetLoader::GetAssetStatusName( EAssetStatus status )
{
	switch (status)
	{
		case ASSET_STATUS_UNLOADED:	return "Unloaded";
		case ASSET_STATUS_LOADING:	return "Loading";
		case ASSET_STATUS_LOADED:	return "Loaded";
		case ASSET_STATUS_ERROR:	return "Error";
		default:					return "Unknown";
	}
}

} // namespace Asset

#endif // __GEL_ASSETLOADER_H

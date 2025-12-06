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
**	File name:		AssetLoader.cpp											**
**																			**
**	Created by:		PC Port													**
**																			**
**	Description:	Unified asset loader implementation						**
**																			**
*****************************************************************************/

/*****************************************************************************
**							  	  Includes									**
*****************************************************************************/

#include <gel/AssetLoader/AssetLoader.h>
#include <core/defines.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

/*****************************************************************************
**								   Defines									**
*****************************************************************************/

namespace Asset
{

/*****************************************************************************
**								  Singleton									**
*****************************************************************************/

DefineSingletonClass( CAssetLoader, "Asset Loader" );

/*****************************************************************************
**								Implementation								**
*****************************************************************************/

CAssetLoader::CAssetLoader( void )
{
	mp_assetTable = NULL;
	m_initialized = false;
}

CAssetLoader::~CAssetLoader( void )
{
	Deinit();
}

// Initialize asset loader
void CAssetLoader::Init( void )
{
	if (m_initialized)
		return;
	
	// Create asset hash table
	mp_assetTable = new Lst::HashTable<AssetData>(256);
	
	m_initialized = true;
	printf("AssetLoader: Initialized\n");
}

// Deinitialize asset loader
void CAssetLoader::Deinit( void )
{
	if (!m_initialized)
		return;
	
	// Unload all assets
	UnloadAllAssets(true);
	
	// Delete hash table
	if (mp_assetTable)
	{
		delete mp_assetTable;
		mp_assetTable = NULL;
	}
	
	m_initialized = false;
	printf("AssetLoader: Deinitialized\n");
}

// Calculate simple checksum from string
uint32 CAssetLoader::CalculateChecksum( const char* str )
{
	if (!str)
		return 0;
	
	uint32 checksum = 0;
	while (*str)
	{
		checksum = (checksum << 5) + checksum + (uint32)*str;
		str++;
	}
	return checksum;
}

// Get file extension
const char* CAssetLoader::GetFileExtension( const char* filename )
{
	if (!filename)
		return NULL;
	
	const char* ext = filename;
	while (*filename)
	{
		if (*filename == '.')
			ext = filename + 1;
		filename++;
	}
	return ext;
}

// Detect asset type from filename
EAssetType CAssetLoader::DetectAssetType( const char* filename )
{
	const char* ext = GetFileExtension(filename);
	if (!ext)
		return ASSET_TYPE_UNKNOWN;
	
	// Audio formats
	if (strcmp(ext, "wav") == 0 || strcmp(ext, "WAV") == 0 ||
		strcmp(ext, "ogg") == 0 || strcmp(ext, "OGG") == 0 ||
		strcmp(ext, "mp3") == 0 || strcmp(ext, "MP3") == 0 ||
		strcmp(ext, "flac") == 0 || strcmp(ext, "FLAC") == 0)
	{
		return ASSET_TYPE_AUDIO;
	}
	
	// Video formats
	if (strcmp(ext, "mp4") == 0 || strcmp(ext, "MP4") == 0 ||
		strcmp(ext, "avi") == 0 || strcmp(ext, "AVI") == 0 ||
		strcmp(ext, "mov") == 0 || strcmp(ext, "MOV") == 0 ||
		strcmp(ext, "wmv") == 0 || strcmp(ext, "WMV") == 0)
	{
		return ASSET_TYPE_VIDEO;
	}
	
	return ASSET_TYPE_UNKNOWN;
}

// Get audio format from filename
EAudioFormat CAssetLoader::GetAudioFormat( const char* filename )
{
	const char* ext = GetFileExtension(filename);
	if (!ext)
		return AUDIO_FORMAT_UNKNOWN;
	
	if (strcmp(ext, "wav") == 0 || strcmp(ext, "WAV") == 0)
		return AUDIO_FORMAT_WAV;
	if (strcmp(ext, "ogg") == 0 || strcmp(ext, "OGG") == 0)
		return AUDIO_FORMAT_OGG;
	if (strcmp(ext, "mp3") == 0 || strcmp(ext, "MP3") == 0)
		return AUDIO_FORMAT_MP3;
	if (strcmp(ext, "flac") == 0 || strcmp(ext, "FLAC") == 0)
		return AUDIO_FORMAT_FLAC;
	
	return AUDIO_FORMAT_UNKNOWN;
}

// Get video format from filename
EVideoFormat CAssetLoader::GetVideoFormat( const char* filename )
{
	const char* ext = GetFileExtension(filename);
	if (!ext)
		return VIDEO_FORMAT_UNKNOWN;
	
	if (strcmp(ext, "mp4") == 0 || strcmp(ext, "MP4") == 0)
		return VIDEO_FORMAT_MP4;
	if (strcmp(ext, "avi") == 0 || strcmp(ext, "AVI") == 0)
		return VIDEO_FORMAT_AVI;
	if (strcmp(ext, "mov") == 0 || strcmp(ext, "MOV") == 0)
		return VIDEO_FORMAT_MOV;
	if (strcmp(ext, "wmv") == 0 || strcmp(ext, "WMV") == 0)
		return VIDEO_FORMAT_WMV;
	
	return VIDEO_FORMAT_UNKNOWN;
}

// Create asset data structure
AssetData* CAssetLoader::CreateAssetData( const char* filename, EAssetType type, uint32 flags )
{
	if (!m_initialized || !filename)
		return NULL;
	
	// Create new asset data
	AssetData* pAsset = new AssetData();
	pAsset->checksum = CalculateChecksum(filename);
	strncpy(pAsset->filename, filename, sizeof(pAsset->filename) - 1);
	pAsset->type = type;
	pAsset->status = ASSET_STATUS_UNLOADED;
	pAsset->flags = flags;
	pAsset->refCount = 1;
	
	return pAsset;
}

// Load audio data (placeholder - needs actual decoder)
bool CAssetLoader::LoadAudioData( AssetData* pAsset )
{
	if (!pAsset)
		return false;
	
	printf("AssetLoader: Loading audio '%s' (format detection only)\n", pAsset->filename);
	
	// Check if file exists
	FILE* file = fopen(pAsset->filename, "rb");
	if (!file)
	{
		printf("AssetLoader: Failed to open audio file '%s'\n", pAsset->filename);
		return false;
	}
	
	// Get file size
	fseek(file, 0, SEEK_END);
	size_t fileSize = ftell(file);
	fseek(file, 0, SEEK_SET);
	fclose(file);
	
	// Store metadata (actual decoding would happen here with proper library)
	pAsset->dataSize = fileSize;
	pAsset->status = ASSET_STATUS_LOADED;
	
	printf("AssetLoader: Audio file size: %zu bytes (decoder library needed for actual loading)\n", fileSize);
	return true;
}

// Load video data (placeholder - needs FFmpeg or similar)
bool CAssetLoader::LoadVideoData( AssetData* pAsset )
{
	if (!pAsset)
		return false;
	
	printf("AssetLoader: Loading video '%s' (format detection only)\n", pAsset->filename);
	
	// Check if file exists
	FILE* file = fopen(pAsset->filename, "rb");
	if (!file)
	{
		printf("AssetLoader: Failed to open video file '%s'\n", pAsset->filename);
		return false;
	}
	
	// Get file size
	fseek(file, 0, SEEK_END);
	size_t fileSize = ftell(file);
	fseek(file, 0, SEEK_SET);
	fclose(file);
	
	// Store metadata (actual decoding would happen here with FFmpeg)
	pAsset->dataSize = fileSize;
	pAsset->status = ASSET_STATUS_LOADED;
	
	printf("AssetLoader: Video file size: %zu bytes (FFmpeg library needed for actual loading)\n", fileSize);
	return true;
}

// Free asset data
void CAssetLoader::FreeAssetData( AssetData* pAsset )
{
	if (!pAsset)
		return;
	
	// Free loaded data
	if (pAsset->data)
	{
		free(pAsset->data);
		pAsset->data = NULL;
	}
	
	pAsset->dataSize = 0;
	pAsset->status = ASSET_STATUS_UNLOADED;
}

// Load asset by filename
AssetData* CAssetLoader::LoadAsset( const char* filename, EAssetType type, uint32 flags )
{
	if (!m_initialized || !filename)
		return NULL;
	
	// Calculate checksum
	uint32 checksum = CalculateChecksum(filename);
	
	// Check if already loaded
	AssetData* pAsset = mp_assetTable->GetItem(checksum);
	if (pAsset)
	{
		// Already loaded, increment ref count
		pAsset->refCount++;
		printf("AssetLoader: Asset '%s' already loaded (refCount=%d)\n", filename, pAsset->refCount);
		return pAsset;
	}
	
	// Auto-detect type if unknown
	if (type == ASSET_TYPE_UNKNOWN)
	{
		type = DetectAssetType(filename);
	}
	
	// Create asset data
	pAsset = CreateAssetData(filename, type, flags);
	if (!pAsset)
		return NULL;
	
	// Load based on type
	bool success = false;
	pAsset->status = ASSET_STATUS_LOADING;
	
	switch (type)
	{
		case ASSET_TYPE_AUDIO:
			success = LoadAudioData(pAsset);
			break;
			
		case ASSET_TYPE_VIDEO:
			success = LoadVideoData(pAsset);
			break;
			
		default:
			printf("AssetLoader: Unsupported asset type %d\n", type);
			break;
	}
	
	if (success)
	{
		// Add to hash table
		mp_assetTable->PutItem(checksum, pAsset);
		printf("AssetLoader: Successfully loaded '%s' (%s)\n", filename, GetAssetTypeName(type));
	}
	else
	{
		pAsset->status = ASSET_STATUS_ERROR;
		delete pAsset;
		pAsset = NULL;
		printf("AssetLoader: Failed to load '%s'\n", filename);
	}
	
	return pAsset;
}

// Load asset by checksum
AssetData* CAssetLoader::LoadAsset( uint32 checksum, EAssetType type, uint32 flags )
{
	if (!m_initialized)
		return NULL;
	
	// Try to find existing asset
	AssetData* pAsset = mp_assetTable->GetItem(checksum);
	if (pAsset)
	{
		pAsset->refCount++;
		return pAsset;
	}
	
	printf("AssetLoader: Cannot load by checksum alone - filename required\n");
	return NULL;
}

// Unload asset
void CAssetLoader::UnloadAsset( AssetData* pAsset )
{
	if (!m_initialized || !pAsset)
		return;
	
	// Decrement ref count
	pAsset->refCount--;
	
	// Only unload if ref count reaches zero and not permanent
	if (pAsset->refCount <= 0 && !(pAsset->flags & ASSET_LOAD_PERMANENT))
	{
		printf("AssetLoader: Unloading '%s'\n", pAsset->filename);
		
		// Remove from hash table
		mp_assetTable->FlushItem(pAsset->checksum);
		
		// Free data
		FreeAssetData(pAsset);
		delete pAsset;
	}
}

// Unload asset by checksum
void CAssetLoader::UnloadAsset( uint32 checksum )
{
	if (!m_initialized)
		return;
	
	AssetData* pAsset = mp_assetTable->GetItem(checksum);
	if (pAsset)
	{
		UnloadAsset(pAsset);
	}
}

// Unload all assets
void CAssetLoader::UnloadAllAssets( bool includePermanent )
{
	if (!m_initialized || !mp_assetTable)
		return;
	
	printf("AssetLoader: Unloading all assets%s\n", includePermanent ? " (including permanent)" : "");
	
	// Note: This is a simplified version
	// A proper implementation would iterate through the hash table
	mp_assetTable->FlushAllItems();
}

// Get asset by filename
AssetData* CAssetLoader::GetAsset( const char* filename )
{
	if (!m_initialized || !filename)
		return NULL;
	
	uint32 checksum = CalculateChecksum(filename);
	return mp_assetTable->GetItem(checksum);
}

// Get asset by checksum
AssetData* CAssetLoader::GetAsset( uint32 checksum )
{
	if (!m_initialized)
		return NULL;
	
	return mp_assetTable->GetItem(checksum);
}

// Check if asset is loaded by filename
bool CAssetLoader::IsAssetLoaded( const char* filename )
{
	AssetData* pAsset = GetAsset(filename);
	return (pAsset && pAsset->status == ASSET_STATUS_LOADED);
}

// Check if asset is loaded by checksum
bool CAssetLoader::IsAssetLoaded( uint32 checksum )
{
	AssetData* pAsset = GetAsset(checksum);
	return (pAsset && pAsset->status == ASSET_STATUS_LOADED);
}

// Add reference
void CAssetLoader::AddRef( AssetData* pAsset )
{
	if (pAsset)
	{
		pAsset->refCount++;
	}
}

// Release reference
void CAssetLoader::Release( AssetData* pAsset )
{
	if (pAsset)
	{
		UnloadAsset(pAsset);
	}
}

// Load audio asset
AudioAssetData* CAssetLoader::LoadAudioAsset( const char* filename, uint32 flags )
{
	AssetData* pAsset = LoadAsset(filename, ASSET_TYPE_AUDIO, flags);
	if (!pAsset)
		return NULL;
	
	// Create audio-specific data
	AudioAssetData* pAudioData = new AudioAssetData();
	pAudioData->format = GetAudioFormat(filename);
	
	// Would populate other fields from actual file parsing
	printf("AssetLoader: Audio format detected: %d\n", pAudioData->format);
	
	return pAudioData;
}

// Load video asset
VideoAssetData* CAssetLoader::LoadVideoAsset( const char* filename, uint32 flags )
{
	AssetData* pAsset = LoadAsset(filename, ASSET_TYPE_VIDEO, flags);
	if (!pAsset)
		return NULL;
	
	// Create video-specific data
	VideoAssetData* pVideoData = new VideoAssetData();
	pVideoData->format = GetVideoFormat(filename);
	
	// Would populate other fields from actual file parsing with FFmpeg
	printf("AssetLoader: Video format detected: %d\n", pVideoData->format);
	
	return pVideoData;
}

// Get number of loaded assets
int CAssetLoader::GetNumLoadedAssets( void )
{
	if (!m_initialized || !mp_assetTable)
		return 0;
	
	// This is a simplified version
	// A proper implementation would count items in hash table
	return 0;
}

// Get total memory usage
size_t CAssetLoader::GetTotalMemoryUsage( void )
{
	if (!m_initialized || !mp_assetTable)
		return 0;
	
	size_t total = 0;
	// Would iterate through all assets and sum dataSize
	return total;
}

// Print statistics
void CAssetLoader::PrintStatistics( void )
{
	printf("=== Asset Loader Statistics ===\n");
	printf("Number of loaded assets: %d\n", GetNumLoadedAssets());
	printf("Total memory usage: %zu bytes\n", GetTotalMemoryUsage());
	printf("================================\n");
}

} // namespace Asset

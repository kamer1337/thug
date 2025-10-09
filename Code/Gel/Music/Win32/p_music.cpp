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
**	Module:			Music (Pcm)												**
**																			**
**	File name:		p_music.cpp												**
**																			**
**	Created by:		PC Port													**
**																			**
**	Description:	PC platform music implementation using HAL				**
**																			**
*****************************************************************************/

/*****************************************************************************
**							  	  Includes									**
*****************************************************************************/

#include <core/defines.h>
#include <gel/music/Win32/p_music.h>
#include <gel/music/music.h>
#include <core/HAL/Audio.h>

/*****************************************************************************
**								   Defines									**
*****************************************************************************/

// For compatibility with the inline stubs, we need to provide implementations
// that were previously inline. We'll include the header to get the inline
// definitions, but provide real implementations here that override them.

namespace Pcm
{

/*****************************************************************************
**							   Public Functions								**
*****************************************************************************/

void PCMAudio_Init( void )
{
	Dbg_Message("Initializing Win32 PCM Audio using HAL...");
	
	HAL::AudioDevice* device = HAL::GetAudioDevice();
	if (!device)
	{
		Dbg_Message("Warning: No audio device available");
		return;
	}
	
	Dbg_Message("PCM Audio initialized with %d streams", device->GetNumStreams());
}

void PCMAudio_Deinit( void )
{
	Dbg_Message("Deinitializing Win32 PCM Audio...");
	
	HAL::AudioDevice* device = HAL::GetAudioDevice();
	if (device)
	{
		device->StopMusic();
	}
}

void PCMAudio_Update( void )
{
	HAL::AudioDevice* device = HAL::GetAudioDevice();
	if (device)
	{
		device->Update();
	}
}

bool PCMAudio_TrackExists( const char* nameOfFile, int trackNumber )
{
	// For now, assume all tracks exist
	// A proper implementation would check for file existence
	return true;
}

bool PCMAudio_LoadMusicHeader( const char* nameOfFile )
{
	HAL::AudioDevice* device = HAL::GetAudioDevice();
	if (!device)
		return false;
	
	return device->LoadMusicHeader(nameOfFile);
}

bool PCMAudio_PreLoadMusicStream( uint32 checksum )
{
	// Preload support not yet implemented
	return false;
}

bool PCMAudio_StartStreaming( void )
{
	// Start streaming support not yet implemented
	return false;
}

bool PCMAudio_StopStreaming( bool wait_for_last_write )
{
	// Stop streaming support not yet implemented
	return false;
}

bool PCMAudio_PauseStream( bool pause )
{
	HAL::AudioDevice* device = HAL::GetAudioDevice();
	if (!device)
		return false;
	
	device->PauseMusic(pause);
	return true;
}

bool PCMAudio_SetVolume( float volume )
{
	HAL::AudioDevice* device = HAL::GetAudioDevice();
	if (!device)
		return false;
	
	device->SetMusicVolume(volume);
	return true;
}

float PCMAudio_GetVolume( void )
{
	HAL::AudioDevice* device = HAL::GetAudioDevice();
	if (!device)
		return 0.0f;
	
	// Get music volume (not implemented in device yet, so return a default)
	return 100.0f;
}

void PCMAudio_SetMusicVolume( float volume )
{
	HAL::AudioDevice* device = HAL::GetAudioDevice();
	if (device)
	{
		device->SetMusicVolume(volume);
	}
}

float PCMAudio_GetMusicVolume( void )
{
	// Return a default for now
	return 100.0f;
}

bool PCMAudio_PlayMusicTrack( const char* filename )
{
	HAL::AudioDevice* device = HAL::GetAudioDevice();
	if (!device)
		return false;
	
	return device->PlayMusicTrack(filename);
}

void PCMAudio_StopMusic( bool fadeOut )
{
	HAL::AudioDevice* device = HAL::GetAudioDevice();
	if (device)
	{
		device->StopMusic();
	}
}

void PCMAudio_Pause( bool pause, int channel )
{
	HAL::AudioDevice* device = HAL::GetAudioDevice();
	if (!device)
		return;
	
	// Pause music or streams based on channel
	if (channel == 0)  // Music channel
	{
		device->PauseMusic(pause);
	}
	// TODO: Handle stream channels
}

int PCMAudio_GetMusicStatus( void )
{
	HAL::AudioDevice* device = HAL::GetAudioDevice();
	if (!device)
		return PCM_STATUS_FREE;
	
	HAL::AudioStatus status = device->GetMusicStatus();
	
	switch (status)
	{
		case HAL::AUDIO_STATUS_FREE:
			return PCM_STATUS_FREE;
		case HAL::AUDIO_STATUS_LOADING:
			return PCM_STATUS_LOADING;
		case HAL::AUDIO_STATUS_PLAYING:
			return PCM_STATUS_PLAYING;
		case HAL::AUDIO_STATUS_PAUSED:
			return PCM_STATUS_PAUSED;
		default:
			return PCM_STATUS_FREE;
	}
}

bool PCMAudio_LoadStreamHeader( const char* filename )
{
	HAL::AudioDevice* device = HAL::GetAudioDevice();
	if (!device)
		return false;
	
	return device->LoadStreamHeader(filename);
}

bool PCMAudio_PlayStream( uint32 checksum, int channel, float volumeL, float volumeR, float pitch )
{
	HAL::AudioDevice* device = HAL::GetAudioDevice();
	if (!device)
		return false;
	
	return device->PlayStream(checksum, channel, volumeL, volumeR, pitch);
}

void PCMAudio_StopStream( int channel, bool waitForStop )
{
	HAL::AudioDevice* device = HAL::GetAudioDevice();
	if (device)
	{
		device->StopStream(channel);
	}
}

void PCMAudio_StopStreams( void )
{
	HAL::AudioDevice* device = HAL::GetAudioDevice();
	if (!device)
		return;
	
	// Stop all streams
	int numStreams = device->GetNumStreams();
	for (int i = 0; i < numStreams; i++)
	{
		device->StopStream(i);
	}
}

void PCMAudio_SetStreamVolume( float volumeL, float volumeR, int channel )
{
	HAL::AudioDevice* device = HAL::GetAudioDevice();
	if (device)
	{
		device->SetStreamVolume(channel, volumeL, volumeR);
	}
}

void PCMAudio_SetStreamPitch( float pitch, int channel )
{
	HAL::AudioDevice* device = HAL::GetAudioDevice();
	if (device)
	{
		device->SetStreamPitch(channel, pitch);
	}
}

int PCMAudio_GetStreamStatus( int channel )
{
	HAL::AudioDevice* device = HAL::GetAudioDevice();
	if (!device)
		return PCM_STATUS_FREE;
	
	HAL::AudioStatus status = device->GetStreamStatus(channel);
	
	switch (status)
	{
		case HAL::AUDIO_STATUS_FREE:
			return PCM_STATUS_FREE;
		case HAL::AUDIO_STATUS_LOADING:
			return PCM_STATUS_LOADING;
		case HAL::AUDIO_STATUS_PLAYING:
			return PCM_STATUS_PLAYING;
		case HAL::AUDIO_STATUS_PAUSED:
			return PCM_STATUS_PAUSED;
		default:
			return PCM_STATUS_FREE;
	}
}

uint32 PCMAudio_FindNameFromChecksum( uint32 checksum, int channel )
{
	// File lookup not yet implemented
	return 0;
}

bool PCMAudio_PreLoadStream( uint32 checksum, int channel )
{
	// Preload not yet implemented
	return false;
}

bool PCMAudio_PreLoadStreamDone( int channel )
{
	// Preload not yet implemented
	return true;
}

bool PCMAudio_StartPreLoadedStream( int channel, float volumeL, float volumeR, float pitch )
{
	// Preload not yet implemented
	return false;
}

bool PCMAudio_PreLoadMusicStreamDone( void )
{
	// Preload not yet implemented
	return true;
}

bool PCMAudio_StartPreLoadedMusicStream( void )
{
	// Preload not yet implemented
	return false;
}

} // namespace Pcm

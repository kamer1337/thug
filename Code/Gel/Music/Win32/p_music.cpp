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
**	Description:	PC platform music stub implementations					**
**																			**
*****************************************************************************/

/*****************************************************************************
**							  	  Includes									**
*****************************************************************************/

#include <gel/music/Win32/p_music.h>
#include <core/defines.h>

/*****************************************************************************
**								   Defines									**
*****************************************************************************/

namespace Pcm
{

/*****************************************************************************
**							   Public Functions								**
*****************************************************************************/

// Initialize PCM audio system
void PCMAudio_Init( void )
{
#if defined(USE_SDL2_AUDIO)
	SDL2::Audio_Init();
#elif defined(USE_OPENAL_AUDIO)
	OpenAL::Audio_Init();
#elif defined(USE_FMOD_AUDIO)
	FMOD::Audio_Init();
#elif defined(USE_DIRECTSOUND_AUDIO)
	DirectSound::Audio_Init();
#else
	// No audio backend selected - stub implementation
#endif
}

// Deinitialize PCM audio system
void PCMAudio_Deinit( void )
{
#if defined(USE_SDL2_AUDIO)
	SDL2::Audio_Deinit();
#elif defined(USE_OPENAL_AUDIO)
	OpenAL::Audio_Deinit();
#elif defined(USE_FMOD_AUDIO)
	FMOD::Audio_Deinit();
#elif defined(USE_DIRECTSOUND_AUDIO)
	DirectSound::Audio_Deinit();
#else
	// No audio backend selected - stub implementation
#endif
}

// Per-frame update
void PCMAudio_Update( void )
{
#if defined(USE_SDL2_AUDIO)
	SDL2::Audio_Update();
#elif defined(USE_OPENAL_AUDIO)
	OpenAL::Audio_Update();
#elif defined(USE_FMOD_AUDIO)
	FMOD::Audio_Update();
#elif defined(USE_DIRECTSOUND_AUDIO)
	DirectSound::Audio_Update();
#else
	// No audio backend selected - stub implementation
#endif
}

// Check if a music track exists
bool PCMAudio_TrackExists( const char* nameOfFile, int trackNumber )
{
#if defined(USE_SDL2_AUDIO)
	return SDL2::Audio_TrackExists(nameOfFile, trackNumber);
#elif defined(USE_OPENAL_AUDIO)
	return OpenAL::Audio_TrackExists(nameOfFile, trackNumber);
#elif defined(USE_FMOD_AUDIO)
	return FMOD::Audio_TrackExists(nameOfFile, trackNumber);
#elif defined(USE_DIRECTSOUND_AUDIO)
	return DirectSound::Audio_TrackExists(nameOfFile, trackNumber);
#else
	// No audio backend selected - stub implementation
	return false;
#endif
}

// Load music header
bool PCMAudio_LoadMusicHeader( const char* nameOfFile )
{
#if defined(USE_SDL2_AUDIO)
	return SDL2::Audio_LoadMusicHeader(nameOfFile);
#elif defined(USE_OPENAL_AUDIO)
	return OpenAL::Audio_LoadMusicHeader(nameOfFile);
#elif defined(USE_FMOD_AUDIO)
	return FMOD::Audio_LoadMusicHeader(nameOfFile);
#elif defined(USE_DIRECTSOUND_AUDIO)
	return DirectSound::Audio_LoadMusicHeader(nameOfFile);
#else
	// No audio backend selected - stub implementation
	return false;
#endif
}

// Preload music stream
bool PCMAudio_PreLoadMusicStream( uint32 checksum )
{
#if defined(USE_SDL2_AUDIO)
	return SDL2::Audio_PreLoadMusicStream(checksum);
#elif defined(USE_OPENAL_AUDIO)
	return OpenAL::Audio_PreLoadMusicStream(checksum);
#elif defined(USE_FMOD_AUDIO)
	return FMOD::Audio_PreLoadMusicStream(checksum);
#elif defined(USE_DIRECTSOUND_AUDIO)
	return DirectSound::Audio_PreLoadMusicStream(checksum);
#else
	// No audio backend selected - stub implementation
	return false;
#endif
}

// Start streaming music
bool PCMAudio_StartStreaming( void )
{
#if defined(USE_SDL2_AUDIO)
	return SDL2::Audio_StartStreaming();
#elif defined(USE_OPENAL_AUDIO)
	return OpenAL::Audio_StartStreaming();
#elif defined(USE_FMOD_AUDIO)
	return FMOD::Audio_StartStreaming();
#elif defined(USE_DIRECTSOUND_AUDIO)
	return DirectSound::Audio_StartStreaming();
#else
	// No audio backend selected - stub implementation
	return true; // Return true to avoid blocking game
#endif
}

// Stop streaming music
bool PCMAudio_StopStreaming( bool wait_for_last_write )
{
#if defined(USE_SDL2_AUDIO)
	return SDL2::Audio_StopStreaming(wait_for_last_write);
#elif defined(USE_OPENAL_AUDIO)
	return OpenAL::Audio_StopStreaming(wait_for_last_write);
#elif defined(USE_FMOD_AUDIO)
	return FMOD::Audio_StopStreaming(wait_for_last_write);
#elif defined(USE_DIRECTSOUND_AUDIO)
	return DirectSound::Audio_StopStreaming(wait_for_last_write);
#else
	// No audio backend selected - stub implementation
	return true;
#endif
}

// Pause/unpause music stream
bool PCMAudio_PauseStream( bool pause )
{
#if defined(USE_SDL2_AUDIO)
	return SDL2::Audio_PauseStream(pause);
#elif defined(USE_OPENAL_AUDIO)
	return OpenAL::Audio_PauseStream(pause);
#elif defined(USE_FMOD_AUDIO)
	return FMOD::Audio_PauseStream(pause);
#elif defined(USE_DIRECTSOUND_AUDIO)
	return DirectSound::Audio_PauseStream(pause);
#else
	// No audio backend selected - stub implementation
	return true;
#endif
}

// Set stream volume
bool PCMAudio_SetVolume( float volume )
{
#if defined(USE_SDL2_AUDIO)
	return SDL2::Audio_SetVolume(volume);
#elif defined(USE_OPENAL_AUDIO)
	return OpenAL::Audio_SetVolume(volume);
#elif defined(USE_FMOD_AUDIO)
	return FMOD::Audio_SetVolume(volume);
#elif defined(USE_DIRECTSOUND_AUDIO)
	return DirectSound::Audio_SetVolume(volume);
#else
	// No audio backend selected - stub implementation
	return true;
#endif
}

// Get stream volume
float PCMAudio_GetVolume( void )
{
#if defined(USE_SDL2_AUDIO)
	return SDL2::Audio_GetVolume();
#elif defined(USE_OPENAL_AUDIO)
	return OpenAL::Audio_GetVolume();
#elif defined(USE_FMOD_AUDIO)
	return FMOD::Audio_GetVolume();
#elif defined(USE_DIRECTSOUND_AUDIO)
	return DirectSound::Audio_GetVolume();
#else
	// No audio backend selected - stub implementation
	return 1.0f;
#endif
}

// Set music volume
void PCMAudio_SetMusicVolume( float volume )
{
#if defined(USE_SDL2_AUDIO)
	SDL2::Audio_SetMusicVolume(volume);
#elif defined(USE_OPENAL_AUDIO)
	OpenAL::Audio_SetMusicVolume(volume);
#elif defined(USE_FMOD_AUDIO)
	FMOD::Audio_SetMusicVolume(volume);
#elif defined(USE_DIRECTSOUND_AUDIO)
	DirectSound::Audio_SetMusicVolume(volume);
#else
	// No audio backend selected - stub implementation
#endif
}

// Get music volume
float PCMAudio_GetMusicVolume( void )
{
#if defined(USE_SDL2_AUDIO)
	return SDL2::Audio_GetMusicVolume();
#elif defined(USE_OPENAL_AUDIO)
	return OpenAL::Audio_GetMusicVolume();
#elif defined(USE_FMOD_AUDIO)
	return FMOD::Audio_GetMusicVolume();
#elif defined(USE_DIRECTSOUND_AUDIO)
	return DirectSound::Audio_GetMusicVolume();
#else
	// No audio backend selected - stub implementation
	return 1.0f;
#endif
}

} // namespace Pcm

/*****************************************************************************
**						   Global Functions (For Xbox compatibility)			**
*****************************************************************************/

// Global DirectSoundDoWork function for Xbox platform compatibility
// This is called from platform-specific code that needs to keep audio processing
// during intensive operations (e.g., park editor rebuilding)
extern "C" void DirectSoundDoWork( void )
{
#if defined(USE_DIRECTSOUND_AUDIO)
	Pcm::DirectSound::DirectSoundDoWork();
#else
	// When not using DirectSound, just call the regular update
	Pcm::PCMAudio_Update();
#endif
}


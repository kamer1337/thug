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
**	Module:			Music (Pcm) - SDL2_mixer Backend						**
**																			**
**	File name:		p_audio.h												**
**																			**
**	Created by:		PC Port													**
**																			**
**	Description:	SDL2_mixer audio backend stub							**
**																			**
*****************************************************************************/

#ifndef __GEL_MUSIC_SDL2_P_AUDIO_H
#define __GEL_MUSIC_SDL2_P_AUDIO_H

/*****************************************************************************
**							  	  Includes									**
*****************************************************************************/

#ifndef __CORE_DEFINES_H
#include <core/defines.h>
#endif

#ifdef USE_SDL2_AUDIO
#include <SDL2/SDL.h>
#include <SDL2/SDL_mixer.h>
#endif

/*****************************************************************************
**								   Defines									**
*****************************************************************************/

#define SDL2_AUDIO_FREQUENCY 44100
#define SDL2_AUDIO_FORMAT MIX_DEFAULT_FORMAT
#define SDL2_AUDIO_CHANNELS 2
#define SDL2_AUDIO_CHUNKSIZE 2048
#define SDL2_MAX_SOUND_CHANNELS 32
#define SDL2_MAX_STREAMS 8

namespace Pcm
{
namespace SDL2
{

/*****************************************************************************
**							   Public Prototypes							**
*****************************************************************************/

// SDL2_mixer audio backend functions
void	Audio_Init( void );
void	Audio_Deinit( void );
void	Audio_Update( void );
bool	Audio_TrackExists( const char* nameOfFile, int trackNumber = 0 );
bool	Audio_LoadMusicHeader( const char* nameOfFile );
bool	Audio_PreLoadMusicStream( uint32 checksum );
bool	Audio_StartStreaming( void );
bool	Audio_StopStreaming( bool wait_for_last_write = true );
bool	Audio_PauseStream( bool pause );
bool	Audio_SetVolume( float volume );
float	Audio_GetVolume( void );
void	Audio_SetMusicVolume( float volume );
float	Audio_GetMusicVolume( void );

// Sound effects functions
bool	LoadSoundEffect( uint32 checksum, const char* filename );
void	UnloadSoundEffect( uint32 checksum );
int		PlaySoundEffect( uint32 checksum, float volume = 1.0f, float pitch = 1.0f );
void	StopSoundEffect( int channel );
void	SetSoundEffectVolume( int channel, float volume );
bool	IsSoundEffectPlaying( int channel );

// 3D positional audio functions
void	Set3DListenerPosition( float x, float y, float z );
void	Set3DListenerOrientation( float forwardX, float forwardY, float forwardZ, 
									   float upX, float upY, float upZ );
void	Set3DSoundPosition( int channel, float x, float y, float z );
void	Set3DSoundAttenuation( int channel, float minDist, float maxDist );

} // namespace SDL2
} // namespace Pcm

#endif	// __GEL_MUSIC_SDL2_P_AUDIO_H

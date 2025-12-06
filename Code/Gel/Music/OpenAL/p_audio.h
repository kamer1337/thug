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
**	Module:			Music (Pcm) - OpenAL Backend							**
**																			**
**	File name:		p_audio.h												**
**																			**
**	Created by:		PC Port													**
**																			**
**	Description:	OpenAL audio backend stub								**
**																			**
*****************************************************************************/

#ifndef __GEL_MUSIC_OPENAL_P_AUDIO_H
#define __GEL_MUSIC_OPENAL_P_AUDIO_H

/*****************************************************************************
**							  	  Includes									**
*****************************************************************************/

#ifndef __CORE_DEFINES_H
#include <core/defines.h>
#endif

#ifdef USE_OPENAL_AUDIO
#include <AL/al.h>
#include <AL/alc.h>
#endif

/*****************************************************************************
**								   Defines									**
*****************************************************************************/

#define OPENAL_MAX_SOURCES 32
#define OPENAL_MAX_BUFFERS 256

namespace Pcm
{
namespace OpenAL
{

/*****************************************************************************
**							   Public Prototypes							**
*****************************************************************************/

// OpenAL audio backend functions
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
bool	LoadSoundEffect( uint32 checksum, const char* filename, bool is3D = false );
void	UnloadSoundEffect( uint32 checksum );
int		PlaySoundEffect( uint32 checksum, float volume = 1.0f, float pitch = 1.0f );
void	StopSoundEffect( int sourceID );
void	SetSoundEffectVolume( int sourceID, float volume );
void	SetSoundEffectPitch( int sourceID, float pitch );
bool	IsSoundEffectPlaying( int sourceID );

// 3D positional audio functions
void	Set3DListenerPosition( float x, float y, float z );
void	Set3DListenerVelocity( float vx, float vy, float vz );
void	Set3DListenerOrientation( float forwardX, float forwardY, float forwardZ,
									   float upX, float upY, float upZ );
void	Set3DSoundPosition( int sourceID, float x, float y, float z );
void	Set3DSoundVelocity( int sourceID, float vx, float vy, float vz );
void	Set3DSoundAttenuation( int sourceID, float rolloffFactor, float referenceDistance, float maxDistance );
void	Set3DSoundCone( int sourceID, float innerAngle, float outerAngle, float outerGain );
void	SetDopplerFactor( float factor );
void	SetSpeedOfSound( float speed );

} // namespace OpenAL
} // namespace Pcm

#endif	// __GEL_MUSIC_OPENAL_P_AUDIO_H

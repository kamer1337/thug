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
**	Module:			Music (Pcm) - FMOD Backend								**
**																			**
**	File name:		p_audio.h												**
**																			**
**	Created by:		PC Port													**
**																			**
**	Description:	FMOD audio backend stub									**
**																			**
*****************************************************************************/

#ifndef __GEL_MUSIC_FMOD_P_AUDIO_H
#define __GEL_MUSIC_FMOD_P_AUDIO_H

/*****************************************************************************
**							  	  Includes									**
*****************************************************************************/

#ifndef __CORE_DEFINES_H
#include <core/defines.h>
#endif

#ifdef USE_FMOD_AUDIO
#include <fmod.hpp>
#include <fmod_errors.h>
#endif

/*****************************************************************************
**								   Defines									**
*****************************************************************************/

#define FMOD_MAX_CHANNELS 64
#define FMOD_MAX_SOUNDS 256

namespace Pcm
{
namespace FMOD
{

/*****************************************************************************
**							   Public Prototypes							**
*****************************************************************************/

// FMOD audio backend functions
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
bool	LoadSoundEffect( uint32 checksum, const char* filename, bool is3D = false, bool streaming = false );
void	UnloadSoundEffect( uint32 checksum );
int		PlaySoundEffect( uint32 checksum, float volume = 1.0f, float pitch = 1.0f );
void	StopSoundEffect( int channelID );
void	SetSoundEffectVolume( int channelID, float volume );
void	SetSoundEffectPitch( int channelID, float pitch );
bool	IsSoundEffectPlaying( int channelID );

// 3D positional audio functions
void	Set3DListenerPosition( float x, float y, float z );
void	Set3DListenerVelocity( float vx, float vy, float vz );
void	Set3DListenerOrientation( float forwardX, float forwardY, float forwardZ,
									   float upX, float upY, float upZ );
void	Set3DSoundPosition( int channelID, float x, float y, float z );
void	Set3DSoundVelocity( int channelID, float vx, float vy, float vz );
void	Set3DSoundMinMaxDistance( int channelID, float minDist, float maxDist );

// DSP effects functions
void	SetReverbProperties( float roomSize, float damping, float wetLevel );
void	SetLowPassFilter( int channelID, float cutoffFrequency );
void	SetHighPassFilter( int channelID, float cutoffFrequency );

} // namespace FMOD
} // namespace Pcm

#endif	// __GEL_MUSIC_FMOD_P_AUDIO_H

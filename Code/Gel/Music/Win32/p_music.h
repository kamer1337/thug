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
**	File name:		p_music.h												**
**																			**
**	Created by:		PC Port													**
**																			**
**	Description:	PC platform music stub									**
**																			**
*****************************************************************************/

#ifndef __GEL_MUSIC_P_MUSIC_H
#define __GEL_MUSIC_P_MUSIC_H

/*****************************************************************************
**							  	  Includes									**
*****************************************************************************/

#ifndef __CORE_DEFINES_H
#include <core/defines.h>
#endif

// Include appropriate audio backend based on compile-time configuration
#if defined(USE_SDL2_AUDIO)
#include <gel/music/SDL2/p_audio.h>
#elif defined(USE_OPENAL_AUDIO)
#include <gel/music/OpenAL/p_audio.h>
#elif defined(USE_FMOD_AUDIO)
#include <gel/music/FMOD/p_audio.h>
#elif defined(USE_DIRECTSOUND_AUDIO)
#include <gel/music/DirectSound/p_audio.h>
#endif

/*****************************************************************************
**								   Defines									**
*****************************************************************************/

namespace Pcm
{

/*****************************************************************************
**							   Public Prototypes							**
*****************************************************************************/

// PC platform music functions
void	PCMAudio_Init( void );
void	PCMAudio_Deinit( void );
void	PCMAudio_Update( void );
bool	PCMAudio_TrackExists( const char* nameOfFile, int trackNumber = 0 );
bool	PCMAudio_LoadMusicHeader( const char* nameOfFile );
bool	PCMAudio_PreLoadMusicStream( uint32 checksum );
bool	PCMAudio_StartStreaming( void );
bool	PCMAudio_StopStreaming( bool wait_for_last_write = true );
bool	PCMAudio_PauseStream( bool pause );
bool	PCMAudio_SetVolume( float volume );
float	PCMAudio_GetVolume( void );
void	PCMAudio_SetMusicVolume( float volume );
float	PCMAudio_GetMusicVolume( void );

} // namespace Pcm

/*****************************************************************************
**						   Global Functions (For Xbox compatibility)			**
*****************************************************************************/

// Global DirectSoundDoWork function for Xbox platform compatibility
extern "C" void DirectSoundDoWork( void );

#endif	// __GEL_MUSIC_P_MUSIC_H

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

// For full implementation, uncomment:
// #include <AL/al.h>
// #include <AL/alc.h>

/*****************************************************************************
**								   Defines									**
*****************************************************************************/

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

} // namespace OpenAL
} // namespace Pcm

#endif	// __GEL_MUSIC_OPENAL_P_AUDIO_H

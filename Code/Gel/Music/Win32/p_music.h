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

/*****************************************************************************
**								   Defines									**
*****************************************************************************/

namespace Pcm
{

/*****************************************************************************
**							   Public Prototypes							**
*****************************************************************************/

// Audio status codes (matching HAL)
#define PCM_STATUS_FREE		0
#define PCM_STATUS_LOADING	1
#define PCM_STATUS_PLAYING	2
#define PCM_STATUS_PAUSED	3

// Channel constants
#define MUSIC_CHANNEL		0
#define EXTRA_CHANNEL		1

// Number of streams available
#define NUM_STREAMS			4

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
bool	PCMAudio_PlayMusicTrack( const char* filename );
void	PCMAudio_StopMusic( bool fadeOut = false );
void	PCMAudio_Pause( bool pause, int channel );
int	PCMAudio_GetMusicStatus( void );
bool	PCMAudio_LoadStreamHeader( const char* filename );
bool	PCMAudio_PlayStream( uint32 checksum, int channel, float volumeL, float volumeR, float pitch );
void	PCMAudio_StopStream( int channel, bool waitForStop = false );
void	PCMAudio_StopStreams( void );
void	PCMAudio_SetStreamVolume( float volumeL, float volumeR, int channel );
void	PCMAudio_SetStreamPitch( float pitch, int channel );
int	PCMAudio_GetStreamStatus( int channel );
uint32	PCMAudio_FindNameFromChecksum( uint32 checksum, int channel );
bool	PCMAudio_PreLoadStream( uint32 checksum, int channel );
bool	PCMAudio_PreLoadStreamDone( int channel );
bool	PCMAudio_StartPreLoadedStream( int channel, float volumeL, float volumeR, float pitch );
bool	PCMAudio_PreLoadMusicStreamDone( void );
bool	PCMAudio_StartPreLoadedMusicStream( void );

} // namespace Pcm

#endif	// __GEL_MUSIC_P_MUSIC_H

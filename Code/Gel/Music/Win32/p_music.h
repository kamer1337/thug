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

// PC platform music stubs
inline void PCMAudio_Init( void ) {}
inline void PCMAudio_Deinit( void ) {}
inline void PCMAudio_Update( void ) {}
inline bool PCMAudio_TrackExists( const char* nameOfFile, int trackNumber = 0 ) { return false; }
inline bool PCMAudio_LoadMusicHeader( const char* nameOfFile ) { return false; }
inline bool PCMAudio_PreLoadMusicStream( uint32 checksum ) { return false; }
inline bool PCMAudio_StartStreaming( void ) { return false; }
inline bool PCMAudio_StopStreaming( bool wait_for_last_write = true ) { return false; }
inline bool PCMAudio_PauseStream( bool pause ) { return false; }
inline bool PCMAudio_SetVolume( float volume ) { return false; }
inline float PCMAudio_GetVolume( void ) { return 0.0f; }
inline void PCMAudio_SetMusicVolume( float volume ) {}
inline float PCMAudio_GetMusicVolume( void ) { return 0.0f; }

} // namespace Pcm

#endif	// __GEL_MUSIC_P_MUSIC_H

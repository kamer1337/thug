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
**	Module:			Movies													**
**																			**
**	File name:		p_movies.cpp											**
**																			**
**	Created by:		PC Port													**
**																			**
**	Description:	PC platform movies stub implementations					**
**																			**
*****************************************************************************/

/*****************************************************************************
**							  	  Includes									**
*****************************************************************************/

#include <gel/movies/Win32/p_movies.h>
#include <core/defines.h>

/*****************************************************************************
**								   Defines									**
*****************************************************************************/

namespace Flx
{

/*****************************************************************************
**							   Private Data									**
*****************************************************************************/

static bool s_movie_playing = false;

/*****************************************************************************
**							   Public Functions								**
*****************************************************************************/

// Play a movie file
bool PlayMovie( const char* pMovieName )
{
	// PC stub - movie playback not implemented
	// In a full implementation, this would use Windows Media Foundation or FFmpeg
	s_movie_playing = false;
	return false;
}

// Check if a movie is currently playing
bool IsPlayingMovie( void )
{
	return s_movie_playing;
}

// Stop the currently playing movie
void StopMovie( void )
{
	// PC stub - movie playback not implemented
	s_movie_playing = false;
}

// Pause/unpause the currently playing movie
void PauseMovie( bool pause )
{
	// PC stub - movie playback not implemented
}

} // namespace Flx

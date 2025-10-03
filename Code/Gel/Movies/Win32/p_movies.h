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
**	File name:		p_movies.h												**
**																			**
**	Created by:		PC Port													**
**																			**
**	Description:	PC platform movies stub									**
**																			**
*****************************************************************************/

#ifndef __GEL_MOVIES_P_MOVIES_H
#define __GEL_MOVIES_P_MOVIES_H

/*****************************************************************************
**							  	  Includes									**
*****************************************************************************/

#ifndef __CORE_DEFINES_H
#include <core/defines.h>
#endif

/*****************************************************************************
**								   Defines									**
*****************************************************************************/

namespace Flx
{

/*****************************************************************************
**							   Public Prototypes							**
*****************************************************************************/

// PC platform movie stubs
inline bool PlayMovie( const char* pMovieName ) { return false; }
inline bool IsPlayingMovie( void ) { return false; }
inline void StopMovie( void ) {}
inline void PauseMovie( bool pause ) {}

} // namespace Flx

#endif	// __GEL_MOVIES_P_MOVIES_H

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
**	Module:			Sound effects  (Sfx)									**
**																			**
**	File name:		p_sfx.h													**
**																			**
**	Created by:		PC Port													**
**																			**
**	Description:	PC platform sound FX stub								**
**																			**
*****************************************************************************/

#ifndef __MODULES_P_SFX_H
#define __MODULES_P_SFX_H

/*****************************************************************************
**							  	  Includes									**
*****************************************************************************/

#ifndef __CORE_DEFINES_H
#include <core/defines.h>
#endif
 
/*****************************************************************************
**								   Defines									**
*****************************************************************************/

namespace Sfx
{

// PC platform - using generic sound system
#define NUM_VOICES		32

struct sVolume;
class CSfxManager;

// Platform specific info needed per wave (stored in the WaveTable).
struct PlatformWaveInfo
{
	void*	p_sound_data;
	bool	looping;
	bool	permanent;
};

/*****************************************************************************
**							  Public Declarations							**
*****************************************************************************/

extern float	gSfxVolume;

/*****************************************************************************
**							   Public Prototypes							**
*****************************************************************************/

void	InitSoundFX( CSfxManager *p_sfx_manager );
void	CleanUpSoundFX( void );
void	StopAllSoundFX( void );
bool	LoadSoundPlease( const char *sfxName, uint32 checksum, PlatformWaveInfo *pInfo, bool loadPerm = 0 );

int		PlaySoundPlease( PlatformWaveInfo *pInfo, sVolume *p_vol, float pitch = 100.0f );
void	StopSoundPlease( int whichVoice );
int		GetMemAvailable( void );
void	PauseSoundsPlease( void );
void	SetReverbPlease( float reverbLevel = 0.0f, int reverbMode = 0, bool instant = false );
void	SetVolumePlease( float volumeLevel );
bool	VoiceIsOn( int whichVoice );
void	SetVoiceParameters( int whichVoice, sVolume *p_vol, float pitch = 0.0 );
void	PerFrameUpdate( void );

} // namespace Sfx

#endif	// __MODULES_P_SFX_H

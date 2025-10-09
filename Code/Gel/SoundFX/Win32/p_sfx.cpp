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
**	File name:		p_sfx.cpp												**
**																			**
**	Created by:		PC Port													**
**																			**
**	Description:	PC platform sound FX implementation using HAL			**
**																			**
*****************************************************************************/

/*****************************************************************************
**							  	  Includes									**
*****************************************************************************/

#include <core/defines.h>
#include <gel/soundfx/Win32/p_sfx.h>
#include <gel/soundfx/soundfx.h>
#include <core/HAL/Audio.h>

/*****************************************************************************
**								   Defines									**
*****************************************************************************/

namespace Sfx
{

/*****************************************************************************
**							   Private Data									**
*****************************************************************************/

float gSfxVolume = 100.0f;

/*****************************************************************************
**							   Public Functions								**
*****************************************************************************/

void InitSoundFX( CSfxManager *p_sfx_manager )
{
	Dbg_Message("Initializing Win32 Sound FX using HAL...");
	
	// HAL audio device should be initialized at app startup
	HAL::AudioDevice* device = HAL::GetAudioDevice();
	if (!device)
	{
		Dbg_Message("Warning: No audio device available");
		return;
	}
	
	Dbg_Message("Sound FX initialized with %d voices", device->GetNumVoices());
}

void CleanUpSoundFX( void )
{
	Dbg_Message("Cleaning up Win32 Sound FX...");
	
	HAL::AudioDevice* device = HAL::GetAudioDevice();
	if (device)
	{
		device->StopAllSounds();
	}
}

void StopAllSoundFX( void )
{
	HAL::AudioDevice* device = HAL::GetAudioDevice();
	if (device)
	{
		device->StopAllSounds();
	}
}

bool LoadSoundPlease( const char *sfxName, uint32 checksum, PlatformWaveInfo *pInfo, bool loadPerm )
{
	if (!pInfo)
		return false;
	
	HAL::AudioDevice* device = HAL::GetAudioDevice();
	if (!device)
		return false;
	
	// Load sound through HAL
	HAL::AudioSound* sound = device->LoadSound(sfxName, pInfo->looping, loadPerm);
	if (!sound)
	{
		Dbg_Message("Failed to load sound: %s", sfxName);
		return false;
	}
	
	// Store in platform info
	pInfo->p_sound_data = sound;
	pInfo->permanent = loadPerm;
	
	return true;
}

int PlaySoundPlease( PlatformWaveInfo *pInfo, sVolume *p_vol, float pitch )
{
	if (!pInfo || !p_vol)
		return -1;
	
	HAL::AudioDevice* device = HAL::GetAudioDevice();
	if (!device)
		return -1;
	
	HAL::AudioSound* sound = (HAL::AudioSound*)pInfo->p_sound_data;
	if (!sound)
		return -1;
	
	// Get volume from sVolume structure
	float volumeL = p_vol->GetChannelVolume(0);
	float volumeR = p_vol->GetChannelVolume(1);
	
	// Play sound
	int voice = device->PlaySound(sound, volumeL, volumeR, pitch);
	
	return voice;
}

void StopSoundPlease( int whichVoice )
{
	HAL::AudioDevice* device = HAL::GetAudioDevice();
	if (device)
	{
		device->StopSound(whichVoice);
	}
}

int GetMemAvailable( void )
{
	// Return a large value since we're not tracking memory usage for PC
	return 64 * 1024 * 1024;  // 64 MB
}

void PauseSoundsPlease( void )
{
	HAL::AudioDevice* device = HAL::GetAudioDevice();
	if (device)
	{
		device->PauseSounds(true);
	}
}

void SetReverbPlease( float reverbLevel, int reverbMode, bool instant )
{
	// Reverb not yet implemented
	// Would require audio effects processing
}

void SetVolumePlease( float volumeLevel )
{
	gSfxVolume = volumeLevel;
	
	HAL::AudioDevice* device = HAL::GetAudioDevice();
	if (device)
	{
		device->SetMasterVolume(volumeLevel);
	}
}

bool VoiceIsOn( int whichVoice )
{
	HAL::AudioDevice* device = HAL::GetAudioDevice();
	if (!device)
		return false;
	
	return device->IsSoundPlaying(whichVoice);
}

void SetVoiceParameters( int whichVoice, sVolume *p_vol, float pitch )
{
	if (!p_vol)
		return;
	
	HAL::AudioDevice* device = HAL::GetAudioDevice();
	if (!device)
		return;
	
	float volumeL = p_vol->GetChannelVolume(0);
	float volumeR = p_vol->GetChannelVolume(1);
	
	device->SetSoundParams(whichVoice, volumeL, volumeR, pitch);
}

void PerFrameUpdate( void )
{
	HAL::AudioDevice* device = HAL::GetAudioDevice();
	if (device)
	{
		device->Update();
	}
}

} // namespace Sfx

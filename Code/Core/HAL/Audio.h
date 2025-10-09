/*****************************************************************************
**																			**
**					   	  Neversoft Entertainment							**
**																		   	**
**				   Copyright (C) 1999 - All Rights Reserved				   	**
**																			**
******************************************************************************
**																			**
**	Project:		THUG (Tony Hawk's Underground)							**
**																			**
**	Module:			HAL (Hardware Abstraction Layer)						**
**																			**
**	File name:		Audio.h													**
**																			**
**	Created:		2024													**
**																			**
**	Description:	Cross-platform audio backend interface					**
**																			**
*****************************************************************************/

#ifndef __CORE_HAL_AUDIO_H
#define __CORE_HAL_AUDIO_H

/*****************************************************************************
**							  	  Includes									**
*****************************************************************************/

#ifndef __CORE_DEFINES_H
#include <core/defines.h>
#endif

/*****************************************************************************
**								   Defines									**
*****************************************************************************/

namespace HAL
{

// Audio backend types
enum AudioBackend
{
	AUDIO_BACKEND_NULL,			// No audio (stub implementation)
	AUDIO_BACKEND_OPENAL,		// OpenAL cross-platform audio
	AUDIO_BACKEND_SDL_MIXER,	// SDL2_mixer simple audio
	AUDIO_BACKEND_FMOD,			// FMOD audio (optional)
};

// Audio status codes
enum AudioStatus
{
	AUDIO_STATUS_FREE = 0,
	AUDIO_STATUS_LOADING,
	AUDIO_STATUS_PLAYING,
	AUDIO_STATUS_PAUSED,
};

// Forward declarations
struct AudioDevice;
struct AudioSound;
struct AudioStream;

/*****************************************************************************
**							  Public Declarations							**
*****************************************************************************/

/**
 * Audio device interface - represents an initialized audio system
 */
struct AudioDevice
{
	virtual ~AudioDevice() {}
	
	// Device management
	virtual bool Initialize() = 0;
	virtual void Shutdown() = 0;
	virtual void Update() = 0;
	
	// Volume control
	virtual void SetMasterVolume(float volume) = 0;
	virtual float GetMasterVolume() = 0;
	
	// Sound effects
	virtual AudioSound* LoadSound(const char* filename, bool looping = false, bool permanent = false) = 0;
	virtual void UnloadSound(AudioSound* sound) = 0;
	virtual int PlaySound(AudioSound* sound, float volumeL, float volumeR, float pitch) = 0;
	virtual void StopSound(int voice) = 0;
	virtual void StopAllSounds() = 0;
	virtual bool IsSoundPlaying(int voice) = 0;
	virtual void SetSoundParams(int voice, float volumeL, float volumeR, float pitch) = 0;
	virtual void PauseSounds(bool pause) = 0;
	
	// Music streaming
	virtual bool LoadMusicHeader(const char* filename) = 0;
	virtual bool PlayMusicTrack(const char* filename) = 0;
	virtual void StopMusic() = 0;
	virtual void PauseMusic(bool pause) = 0;
	virtual void SetMusicVolume(float volume) = 0;
	virtual AudioStatus GetMusicStatus() = 0;
	
	// Stream playback (for sound effects)
	virtual bool LoadStreamHeader(const char* filename) = 0;
	virtual bool PlayStream(uint32 checksum, int channel, float volumeL, float volumeR, float pitch) = 0;
	virtual void StopStream(int channel) = 0;
	virtual void SetStreamVolume(int channel, float volumeL, float volumeR) = 0;
	virtual void SetStreamPitch(int channel, float pitch) = 0;
	virtual AudioStatus GetStreamStatus(int channel) = 0;
	
	// 3D audio support
	virtual void SetListenerPosition(float x, float y, float z) = 0;
	virtual void SetListenerOrientation(float atX, float atY, float atZ, float upX, float upY, float upZ) = 0;
	virtual void SetSoundPosition(int voice, float x, float y, float z) = 0;
	
	// Info
	virtual int GetNumVoices() = 0;
	virtual int GetNumStreams() = 0;
	virtual AudioBackend GetBackendType() = 0;
};

/**
 * Audio sound handle - represents a loaded sound effect
 */
struct AudioSound
{
	void* p_backend_data;		// Backend-specific data
	uint32 checksum;			// Sound identifier
	bool looping;				// Is this a looping sound?
	bool permanent;				// Is this permanently loaded?
	float pitch_adjustment;		// Platform-specific pitch adjustment
};

/**
 * Audio stream handle - represents a music or streaming sound
 */
struct AudioStream
{
	void* p_backend_data;		// Backend-specific data
	uint32 checksum;			// Stream identifier
	int channel;				// Stream channel index
	float volume;				// Current volume
	float pitch;				// Current pitch
};

/*****************************************************************************
**							   Public Prototypes							**
*****************************************************************************/

// Factory function to create appropriate audio device
AudioDevice* CreateAudioDevice(AudioBackend backend = AUDIO_BACKEND_NULL);
void DestroyAudioDevice(AudioDevice* device);

// Get current audio device
AudioDevice* GetAudioDevice();

} // namespace HAL

#endif	// __CORE_HAL_AUDIO_H

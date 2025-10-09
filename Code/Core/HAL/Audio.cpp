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
**	File name:		Audio.cpp												**
**																			**
**	Created:		2024													**
**																			**
**	Description:	Cross-platform audio backend implementation				**
**																			**
*****************************************************************************/

/*****************************************************************************
**							  	  Includes									**
*****************************************************************************/

#include <core/defines.h>
#include <core/HAL/Audio.h>
#include <sys/config/config.h>

#ifdef USE_OPENAL
#include <core/HAL/AudioOpenAL.h>
#endif

/*****************************************************************************
**								   Defines									**
*****************************************************************************/

namespace HAL
{

/*****************************************************************************
**							   Private Data									**
*****************************************************************************/

static AudioDevice* s_audio_device = NULL;

/*****************************************************************************
**							  Private Prototypes							**
*****************************************************************************/

class NullAudioDevice : public AudioDevice
{
public:
	NullAudioDevice() {}
	virtual ~NullAudioDevice() {}
	
	// Device management
	virtual bool Initialize() { return true; }
	virtual void Shutdown() {}
	virtual void Update() {}
	
	// Volume control
	virtual void SetMasterVolume(float volume) {}
	virtual float GetMasterVolume() { return 0.0f; }
	
	// Sound effects
	virtual AudioSound* LoadSound(const char* filename, bool looping, bool permanent) { return NULL; }
	virtual void UnloadSound(AudioSound* sound) {}
	virtual int PlaySound(AudioSound* sound, float volumeL, float volumeR, float pitch) { return -1; }
	virtual void StopSound(int voice) {}
	virtual void StopAllSounds() {}
	virtual bool IsSoundPlaying(int voice) { return false; }
	virtual void SetSoundParams(int voice, float volumeL, float volumeR, float pitch) {}
	virtual void PauseSounds(bool pause) {}
	
	// Music streaming
	virtual bool LoadMusicHeader(const char* filename) { return false; }
	virtual bool PlayMusicTrack(const char* filename) { return false; }
	virtual void StopMusic() {}
	virtual void PauseMusic(bool pause) {}
	virtual void SetMusicVolume(float volume) {}
	virtual AudioStatus GetMusicStatus() { return AUDIO_STATUS_FREE; }
	
	// Stream playback
	virtual bool LoadStreamHeader(const char* filename) { return false; }
	virtual bool PlayStream(uint32 checksum, int channel, float volumeL, float volumeR, float pitch) { return false; }
	virtual void StopStream(int channel) {}
	virtual void SetStreamVolume(int channel, float volumeL, float volumeR) {}
	virtual void SetStreamPitch(int channel, float pitch) {}
	virtual AudioStatus GetStreamStatus(int channel) { return AUDIO_STATUS_FREE; }
	
	// 3D audio support
	virtual void SetListenerPosition(float x, float y, float z) {}
	virtual void SetListenerOrientation(float atX, float atY, float atZ, float upX, float upY, float upZ) {}
	virtual void SetSoundPosition(int voice, float x, float y, float z) {}
	
	// Info
	virtual int GetNumVoices() { return 32; }
	virtual int GetNumStreams() { return 4; }
	virtual AudioBackend GetBackendType() { return AUDIO_BACKEND_NULL; }
};

/*****************************************************************************
**							   Public Functions								**
*****************************************************************************/

/**
 * Create an audio device with the specified backend
 */
AudioDevice* CreateAudioDevice(AudioBackend backend)
{
	if (s_audio_device)
	{
		Dbg_Message("Warning: Audio device already exists");
		return s_audio_device;
	}
	
	switch (backend)
	{
		case AUDIO_BACKEND_OPENAL:
#ifdef USE_OPENAL
			Dbg_Message("Creating OpenAL audio device");
			s_audio_device = new OpenALAudioDevice();
#else
			Dbg_Message("OpenAL backend not compiled in");
			s_audio_device = new NullAudioDevice();
#endif
			break;
			
		case AUDIO_BACKEND_SDL_MIXER:
			Dbg_Message("SDL_mixer backend not yet implemented");
			s_audio_device = new NullAudioDevice();
			break;
			
		case AUDIO_BACKEND_FMOD:
			Dbg_Message("FMOD backend not yet implemented");
			s_audio_device = new NullAudioDevice();
			break;
			
		case AUDIO_BACKEND_NULL:
		default:
			Dbg_Message("Creating NULL audio device (no sound)");
			s_audio_device = new NullAudioDevice();
			break;
	}
	
	if (s_audio_device && !s_audio_device->Initialize())
	{
		Dbg_Message("Failed to initialize audio device");
		delete s_audio_device;
		s_audio_device = NULL;
	}
	
	return s_audio_device;
}

/**
 * Destroy the audio device
 */
void DestroyAudioDevice(AudioDevice* device)
{
	if (device)
	{
		device->Shutdown();
		delete device;
		
		if (device == s_audio_device)
		{
			s_audio_device = NULL;
		}
	}
}

/**
 * Get the current audio device
 */
AudioDevice* GetAudioDevice()
{
	return s_audio_device;
}

} // namespace HAL

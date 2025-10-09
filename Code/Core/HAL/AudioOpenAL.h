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
**	File name:		AudioOpenAL.h											**
**																			**
**	Created:		2024													**
**																			**
**	Description:	OpenAL audio backend implementation						**
**																			**
*****************************************************************************/

#ifndef __CORE_HAL_AUDIO_OPENAL_H
#define __CORE_HAL_AUDIO_OPENAL_H

/*****************************************************************************
**							  	  Includes									**
*****************************************************************************/

#include <core/HAL/Audio.h>

/*****************************************************************************
**								   Defines									**
*****************************************************************************/

namespace HAL
{

/**
 * OpenAL audio device implementation
 */
class OpenALAudioDevice : public AudioDevice
{
public:
	OpenALAudioDevice();
	virtual ~OpenALAudioDevice();
	
	// Device management
	virtual bool Initialize();
	virtual void Shutdown();
	virtual void Update();
	
	// Volume control
	virtual void SetMasterVolume(float volume);
	virtual float GetMasterVolume();
	
	// Sound effects
	virtual AudioSound* LoadSound(const char* filename, bool looping, bool permanent);
	virtual void UnloadSound(AudioSound* sound);
	virtual int PlaySound(AudioSound* sound, float volumeL, float volumeR, float pitch);
	virtual void StopSound(int voice);
	virtual void StopAllSounds();
	virtual bool IsSoundPlaying(int voice);
	virtual void SetSoundParams(int voice, float volumeL, float volumeR, float pitch);
	virtual void PauseSounds(bool pause);
	
	// Music streaming
	virtual bool LoadMusicHeader(const char* filename);
	virtual bool PlayMusicTrack(const char* filename);
	virtual void StopMusic();
	virtual void PauseMusic(bool pause);
	virtual void SetMusicVolume(float volume);
	virtual AudioStatus GetMusicStatus();
	
	// Stream playback
	virtual bool LoadStreamHeader(const char* filename);
	virtual bool PlayStream(uint32 checksum, int channel, float volumeL, float volumeR, float pitch);
	virtual void StopStream(int channel);
	virtual void SetStreamVolume(int channel, float volumeL, float volumeR);
	virtual void SetStreamPitch(int channel, float pitch);
	virtual AudioStatus GetStreamStatus(int channel);
	
	// 3D audio support
	virtual void SetListenerPosition(float x, float y, float z);
	virtual void SetListenerOrientation(float atX, float atY, float atZ, float upX, float upY, float upZ);
	virtual void SetSoundPosition(int voice, float x, float y, float z);
	
	// Info
	virtual int GetNumVoices();
	virtual int GetNumStreams();
	virtual AudioBackend GetBackendType();

private:
	void* m_device;					// ALCdevice*
	void* m_context;				// ALCcontext*
	
	struct VoiceInfo
	{
		unsigned int source;		// ALuint
		AudioSound* sound;
		bool active;
	};
	
	VoiceInfo* m_voices;
	int m_num_voices;
	
	struct StreamInfo
	{
		unsigned int source;		// ALuint
		unsigned int buffers[2];	// ALuint[2] - double buffering
		AudioStream* stream;
		bool active;
	};
	
	StreamInfo* m_streams;
	int m_num_streams;
	
	unsigned int m_music_source;	// ALuint
	bool m_music_playing;
	
	float m_master_volume;
	float m_music_volume;
	
	int FindFreeVoice();
	int FindFreeStream();
	void CleanupVoice(int voice);
	void CleanupStream(int stream);
};

} // namespace HAL

#endif	// __CORE_HAL_AUDIO_OPENAL_H

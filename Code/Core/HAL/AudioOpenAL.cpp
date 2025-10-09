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
**	File name:		AudioOpenAL.cpp											**
**																			**
**	Created:		2024													**
**																			**
**	Description:	OpenAL audio backend implementation						**
**																			**
*****************************************************************************/

/*****************************************************************************
**							  	  Includes									**
*****************************************************************************/

#include <core/defines.h>
#include <core/HAL/AudioOpenAL.h>

#ifdef USE_OPENAL

// OpenAL includes
#ifdef __APPLE__
#include <OpenAL/al.h>
#include <OpenAL/alc.h>
#else
#include <AL/al.h>
#include <AL/alc.h>
#endif

#include <string.h>

/*****************************************************************************
**								   Defines									**
*****************************************************************************/

#define MAX_VOICES 32
#define MAX_STREAMS 4

namespace HAL
{

/*****************************************************************************
**							   Private Helpers								**
*****************************************************************************/

static void CheckALError(const char* context)
{
	ALenum error = alGetError();
	if (error != AL_NO_ERROR)
	{
		Dbg_Message("OpenAL Error in %s: 0x%x", context, error);
	}
}

/*****************************************************************************
**							   Public Functions								**
*****************************************************************************/

OpenALAudioDevice::OpenALAudioDevice()
	: m_device(NULL)
	, m_context(NULL)
	, m_voices(NULL)
	, m_num_voices(MAX_VOICES)
	, m_streams(NULL)
	, m_num_streams(MAX_STREAMS)
	, m_music_source(0)
	, m_music_playing(false)
	, m_master_volume(1.0f)
	, m_music_volume(1.0f)
{
}

OpenALAudioDevice::~OpenALAudioDevice()
{
	Shutdown();
}

bool OpenALAudioDevice::Initialize()
{
	Dbg_Message("Initializing OpenAL audio device...");
	
	// Open default device
	m_device = (void*)alcOpenDevice(NULL);
	if (!m_device)
	{
		Dbg_Message("Failed to open OpenAL device");
		return false;
	}
	
	// Create context
	m_context = (void*)alcCreateContext((ALCdevice*)m_device, NULL);
	if (!m_context)
	{
		Dbg_Message("Failed to create OpenAL context");
		alcCloseDevice((ALCdevice*)m_device);
		m_device = NULL;
		return false;
	}
	
	// Make context current
	if (!alcMakeContextCurrent((ALCcontext*)m_context))
	{
		Dbg_Message("Failed to make OpenAL context current");
		alcDestroyContext((ALCcontext*)m_context);
		alcCloseDevice((ALCdevice*)m_device);
		m_context = NULL;
		m_device = NULL;
		return false;
	}
	
	// Initialize voices
	m_voices = new VoiceInfo[m_num_voices];
	for (int i = 0; i < m_num_voices; i++)
	{
		alGenSources(1, &m_voices[i].source);
		m_voices[i].sound = NULL;
		m_voices[i].active = false;
	}
	CheckALError("Generate voices");
	
	// Initialize streams
	m_streams = new StreamInfo[m_num_streams];
	for (int i = 0; i < m_num_streams; i++)
	{
		alGenSources(1, &m_streams[i].source);
		alGenBuffers(2, m_streams[i].buffers);
		m_streams[i].stream = NULL;
		m_streams[i].active = false;
	}
	CheckALError("Generate streams");
	
	// Initialize music source
	alGenSources(1, &m_music_source);
	CheckALError("Generate music source");
	
	// Set listener orientation (facing -Z, up is +Y)
	ALfloat listenerOri[] = { 0.0f, 0.0f, -1.0f, 0.0f, 1.0f, 0.0f };
	alListenerfv(AL_ORIENTATION, listenerOri);
	alListenerfv(AL_POSITION, (ALfloat[]){ 0.0f, 0.0f, 0.0f });
	alListenerfv(AL_VELOCITY, (ALfloat[]){ 0.0f, 0.0f, 0.0f });
	alListenerf(AL_GAIN, m_master_volume);
	CheckALError("Set listener parameters");
	
	Dbg_Message("OpenAL initialized successfully");
	Dbg_Message("  Voices: %d", m_num_voices);
	Dbg_Message("  Streams: %d", m_num_streams);
	
	return true;
}

void OpenALAudioDevice::Shutdown()
{
	Dbg_Message("Shutting down OpenAL audio device...");
	
	// Stop all sounds
	StopAllSounds();
	StopMusic();
	
	// Clean up voices
	if (m_voices)
	{
		for (int i = 0; i < m_num_voices; i++)
		{
			if (m_voices[i].source != 0)
			{
				alDeleteSources(1, &m_voices[i].source);
			}
		}
		delete[] m_voices;
		m_voices = NULL;
	}
	
	// Clean up streams
	if (m_streams)
	{
		for (int i = 0; i < m_num_streams; i++)
		{
			if (m_streams[i].source != 0)
			{
				alDeleteSources(1, &m_streams[i].source);
			}
			alDeleteBuffers(2, m_streams[i].buffers);
		}
		delete[] m_streams;
		m_streams = NULL;
	}
	
	// Clean up music source
	if (m_music_source != 0)
	{
		alDeleteSources(1, &m_music_source);
		m_music_source = 0;
	}
	
	// Destroy context and device
	if (m_context)
	{
		alcMakeContextCurrent(NULL);
		alcDestroyContext((ALCcontext*)m_context);
		m_context = NULL;
	}
	
	if (m_device)
	{
		alcCloseDevice((ALCdevice*)m_device);
		m_device = NULL;
	}
	
	Dbg_Message("OpenAL shutdown complete");
}

void OpenALAudioDevice::Update()
{
	// Update voice states
	for (int i = 0; i < m_num_voices; i++)
	{
		if (m_voices[i].active)
		{
			ALint state;
			alGetSourcei(m_voices[i].source, AL_SOURCE_STATE, &state);
			if (state == AL_STOPPED)
			{
				CleanupVoice(i);
			}
		}
	}
	
	// Update stream states
	for (int i = 0; i < m_num_streams; i++)
	{
		if (m_streams[i].active)
		{
			ALint state;
			alGetSourcei(m_streams[i].source, AL_SOURCE_STATE, &state);
			if (state == AL_STOPPED)
			{
				CleanupStream(i);
			}
		}
	}
	
	// Update music state
	if (m_music_playing)
	{
		ALint state;
		alGetSourcei(m_music_source, AL_SOURCE_STATE, &state);
		if (state == AL_STOPPED)
		{
			m_music_playing = false;
		}
	}
}

void OpenALAudioDevice::SetMasterVolume(float volume)
{
	m_master_volume = volume / 100.0f;  // Convert from percentage to 0.0-1.0
	if (m_master_volume < 0.0f) m_master_volume = 0.0f;
	if (m_master_volume > 1.0f) m_master_volume = 1.0f;
	
	alListenerf(AL_GAIN, m_master_volume);
	CheckALError("SetMasterVolume");
}

float OpenALAudioDevice::GetMasterVolume()
{
	return m_master_volume * 100.0f;  // Convert back to percentage
}

AudioSound* OpenALAudioDevice::LoadSound(const char* filename, bool looping, bool permanent)
{
	// This is a stub - full implementation would load WAV/OGG files
	// For now, just create a placeholder
	AudioSound* sound = new AudioSound();
	sound->p_backend_data = NULL;
	sound->checksum = 0;
	sound->looping = looping;
	sound->permanent = permanent;
	sound->pitch_adjustment = 1.0f;
	
	Dbg_Message("LoadSound stub: %s", filename);
	
	return sound;
}

void OpenALAudioDevice::UnloadSound(AudioSound* sound)
{
	if (!sound)
		return;
	
	// Free OpenAL buffer if allocated
	if (sound->p_backend_data)
	{
		ALuint buffer = (ALuint)(size_t)sound->p_backend_data;
		alDeleteBuffers(1, &buffer);
	}
	
	delete sound;
}

int OpenALAudioDevice::PlaySound(AudioSound* sound, float volumeL, float volumeR, float pitch)
{
	if (!sound)
		return -1;
	
	int voice = FindFreeVoice();
	if (voice < 0)
	{
		Dbg_Message("No free voices available");
		return -1;
	}
	
	// Calculate stereo volume (average for now)
	float volume = (volumeL + volumeR) * 0.5f / 100.0f;
	if (volume < 0.0f) volume = 0.0f;
	if (volume > 1.0f) volume = 1.0f;
	
	// Calculate pitch
	float actualPitch = (pitch / 100.0f) * sound->pitch_adjustment;
	if (actualPitch < 0.5f) actualPitch = 0.5f;
	if (actualPitch > 2.0f) actualPitch = 2.0f;
	
	// Set source parameters
	ALuint source = m_voices[voice].source;
	alSourcef(source, AL_GAIN, volume);
	alSourcef(source, AL_PITCH, actualPitch);
	alSourcei(source, AL_LOOPING, sound->looping ? AL_TRUE : AL_FALSE);
	
	// TODO: Attach buffer and play
	// For now, mark as active
	m_voices[voice].sound = sound;
	m_voices[voice].active = true;
	
	CheckALError("PlaySound");
	
	return voice;
}

void OpenALAudioDevice::StopSound(int voice)
{
	if (voice < 0 || voice >= m_num_voices)
		return;
	
	if (m_voices[voice].active)
	{
		alSourceStop(m_voices[voice].source);
		CleanupVoice(voice);
	}
}

void OpenALAudioDevice::StopAllSounds()
{
	for (int i = 0; i < m_num_voices; i++)
	{
		if (m_voices[i].active)
		{
			alSourceStop(m_voices[i].source);
			CleanupVoice(i);
		}
	}
}

bool OpenALAudioDevice::IsSoundPlaying(int voice)
{
	if (voice < 0 || voice >= m_num_voices)
		return false;
	
	if (!m_voices[voice].active)
		return false;
	
	ALint state;
	alGetSourcei(m_voices[voice].source, AL_SOURCE_STATE, &state);
	return (state == AL_PLAYING);
}

void OpenALAudioDevice::SetSoundParams(int voice, float volumeL, float volumeR, float pitch)
{
	if (voice < 0 || voice >= m_num_voices)
		return;
	
	if (!m_voices[voice].active)
		return;
	
	// Calculate stereo volume (average for now)
	float volume = (volumeL + volumeR) * 0.5f / 100.0f;
	if (volume < 0.0f) volume = 0.0f;
	if (volume > 1.0f) volume = 1.0f;
	
	// Calculate pitch
	float actualPitch = pitch / 100.0f;
	if (m_voices[voice].sound)
		actualPitch *= m_voices[voice].sound->pitch_adjustment;
	if (actualPitch < 0.5f) actualPitch = 0.5f;
	if (actualPitch > 2.0f) actualPitch = 2.0f;
	
	ALuint source = m_voices[voice].source;
	alSourcef(source, AL_GAIN, volume);
	alSourcef(source, AL_PITCH, actualPitch);
	
	CheckALError("SetSoundParams");
}

void OpenALAudioDevice::PauseSounds(bool pause)
{
	for (int i = 0; i < m_num_voices; i++)
	{
		if (m_voices[i].active)
		{
			if (pause)
				alSourcePause(m_voices[i].source);
			else
				alSourcePlay(m_voices[i].source);
		}
	}
}

bool OpenALAudioDevice::LoadMusicHeader(const char* filename)
{
	// Stub implementation
	Dbg_Message("LoadMusicHeader stub: %s", filename);
	return true;
}

bool OpenALAudioDevice::PlayMusicTrack(const char* filename)
{
	// Stub implementation
	Dbg_Message("PlayMusicTrack stub: %s", filename);
	m_music_playing = true;
	return true;
}

void OpenALAudioDevice::StopMusic()
{
	if (m_music_playing)
	{
		alSourceStop(m_music_source);
		m_music_playing = false;
	}
}

void OpenALAudioDevice::PauseMusic(bool pause)
{
	if (m_music_playing)
	{
		if (pause)
			alSourcePause(m_music_source);
		else
			alSourcePlay(m_music_source);
	}
}

void OpenALAudioDevice::SetMusicVolume(float volume)
{
	m_music_volume = volume / 100.0f;
	if (m_music_volume < 0.0f) m_music_volume = 0.0f;
	if (m_music_volume > 1.0f) m_music_volume = 1.0f;
	
	alSourcef(m_music_source, AL_GAIN, m_music_volume);
	CheckALError("SetMusicVolume");
}

AudioStatus OpenALAudioDevice::GetMusicStatus()
{
	if (!m_music_playing)
		return AUDIO_STATUS_FREE;
	
	ALint state;
	alGetSourcei(m_music_source, AL_SOURCE_STATE, &state);
	
	switch (state)
	{
		case AL_PLAYING:
			return AUDIO_STATUS_PLAYING;
		case AL_PAUSED:
			return AUDIO_STATUS_PAUSED;
		default:
			return AUDIO_STATUS_FREE;
	}
}

bool OpenALAudioDevice::LoadStreamHeader(const char* filename)
{
	// Stub implementation
	Dbg_Message("LoadStreamHeader stub: %s", filename);
	return true;
}

bool OpenALAudioDevice::PlayStream(uint32 checksum, int channel, float volumeL, float volumeR, float pitch)
{
	// Stub implementation
	Dbg_Message("PlayStream stub: checksum=0x%x, channel=%d", checksum, channel);
	return true;
}

void OpenALAudioDevice::StopStream(int channel)
{
	if (channel < 0 || channel >= m_num_streams)
		return;
	
	if (m_streams[channel].active)
	{
		alSourceStop(m_streams[channel].source);
		CleanupStream(channel);
	}
}

void OpenALAudioDevice::SetStreamVolume(int channel, float volumeL, float volumeR)
{
	if (channel < 0 || channel >= m_num_streams)
		return;
	
	if (!m_streams[channel].active)
		return;
	
	float volume = (volumeL + volumeR) * 0.5f / 100.0f;
	if (volume < 0.0f) volume = 0.0f;
	if (volume > 1.0f) volume = 1.0f;
	
	alSourcef(m_streams[channel].source, AL_GAIN, volume);
	CheckALError("SetStreamVolume");
}

void OpenALAudioDevice::SetStreamPitch(int channel, float pitch)
{
	if (channel < 0 || channel >= m_num_streams)
		return;
	
	if (!m_streams[channel].active)
		return;
	
	float actualPitch = pitch / 100.0f;
	if (actualPitch < 0.5f) actualPitch = 0.5f;
	if (actualPitch > 2.0f) actualPitch = 2.0f;
	
	alSourcef(m_streams[channel].source, AL_PITCH, actualPitch);
	CheckALError("SetStreamPitch");
}

AudioStatus OpenALAudioDevice::GetStreamStatus(int channel)
{
	if (channel < 0 || channel >= m_num_streams)
		return AUDIO_STATUS_FREE;
	
	if (!m_streams[channel].active)
		return AUDIO_STATUS_FREE;
	
	ALint state;
	alGetSourcei(m_streams[channel].source, AL_SOURCE_STATE, &state);
	
	switch (state)
	{
		case AL_PLAYING:
			return AUDIO_STATUS_PLAYING;
		case AL_PAUSED:
			return AUDIO_STATUS_PAUSED;
		default:
			return AUDIO_STATUS_FREE;
	}
}

void OpenALAudioDevice::SetListenerPosition(float x, float y, float z)
{
	ALfloat pos[] = { x, y, z };
	alListenerfv(AL_POSITION, pos);
	CheckALError("SetListenerPosition");
}

void OpenALAudioDevice::SetListenerOrientation(float atX, float atY, float atZ, float upX, float upY, float upZ)
{
	ALfloat ori[] = { atX, atY, atZ, upX, upY, upZ };
	alListenerfv(AL_ORIENTATION, ori);
	CheckALError("SetListenerOrientation");
}

void OpenALAudioDevice::SetSoundPosition(int voice, float x, float y, float z)
{
	if (voice < 0 || voice >= m_num_voices)
		return;
	
	if (!m_voices[voice].active)
		return;
	
	ALfloat pos[] = { x, y, z };
	alSourcefv(m_voices[voice].source, AL_POSITION, pos);
	CheckALError("SetSoundPosition");
}

int OpenALAudioDevice::GetNumVoices()
{
	return m_num_voices;
}

int OpenALAudioDevice::GetNumStreams()
{
	return m_num_streams;
}

AudioBackend OpenALAudioDevice::GetBackendType()
{
	return AUDIO_BACKEND_OPENAL;
}

int OpenALAudioDevice::FindFreeVoice()
{
	for (int i = 0; i < m_num_voices; i++)
	{
		if (!m_voices[i].active)
			return i;
	}
	return -1;
}

int OpenALAudioDevice::FindFreeStream()
{
	for (int i = 0; i < m_num_streams; i++)
	{
		if (!m_streams[i].active)
			return i;
	}
	return -1;
}

void OpenALAudioDevice::CleanupVoice(int voice)
{
	if (voice >= 0 && voice < m_num_voices)
	{
		m_voices[voice].sound = NULL;
		m_voices[voice].active = false;
	}
}

void OpenALAudioDevice::CleanupStream(int stream)
{
	if (stream >= 0 && stream < m_num_streams)
	{
		m_streams[stream].stream = NULL;
		m_streams[stream].active = false;
	}
}

} // namespace HAL

#endif // USE_OPENAL

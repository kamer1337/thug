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
**	Module:			Music (Pcm) - OpenAL Backend							**
**																			**
**	File name:		p_audio.cpp												**
**																			**
**	Created by:		PC Port													**
**																			**
**	Description:	OpenAL audio backend implementation with 3D audio		**
**																			**
*****************************************************************************/

/*****************************************************************************
**							  	  Includes									**
*****************************************************************************/

#include <gel/music/OpenAL/p_audio.h>
#include <core/defines.h>

#ifdef USE_OPENAL_AUDIO
#include <AL/al.h>
#include <AL/alc.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#endif

/*****************************************************************************
**								   Defines									**
*****************************************************************************/

namespace Pcm
{
namespace OpenAL
{

#ifdef USE_OPENAL_AUDIO

// Internal structures
struct SoundBuffer
{
	uint32 checksum;
	ALuint bufferID;
	char filename[256];
	bool loaded;
	bool is3D;
};

struct AudioSource
{
	ALuint sourceID;
	uint32 bufferChecksum;
	bool inUse;
	bool is3D;
};

// Internal state
static ALCdevice* s_device = NULL;
static ALCcontext* s_context = NULL;
static bool s_initialized = false;

// Music streaming
static ALuint s_musicSource = 0;
static ALuint s_musicBuffers[4]; // Use 4 buffers for streaming
static bool s_streaming = false;
static float s_stream_volume = 1.0f;
static float s_music_volume = 1.0f;

// Sound effects management
static SoundBuffer s_buffers[OPENAL_MAX_BUFFERS];
static int s_num_buffers = 0;
static AudioSource s_sources[OPENAL_MAX_SOURCES];
static int s_num_sources = 0;

// Helper function to check OpenAL errors
static void CheckALError(const char* operation)
{
	ALenum error = alGetError();
	if (error != AL_NO_ERROR)
	{
		printf("OpenAL Error during %s: 0x%04X\n", operation, error);
	}
}

// Helper function to find or create a source
static int GetAvailableSource()
{
	// Try to find an available source
	for (int i = 0; i < s_num_sources; i++)
	{
		ALint state;
		alGetSourcei(s_sources[i].sourceID, AL_SOURCE_STATE, &state);
		if (state != AL_PLAYING && state != AL_PAUSED)
		{
			s_sources[i].inUse = false;
		}
		
		if (!s_sources[i].inUse)
		{
			return i;
		}
	}
	
	// Create new source if possible
	if (s_num_sources < OPENAL_MAX_SOURCES)
	{
		ALuint sourceID;
		alGenSources(1, &sourceID);
		if (alGetError() == AL_NO_ERROR)
		{
			s_sources[s_num_sources].sourceID = sourceID;
			s_sources[s_num_sources].inUse = false;
			s_sources[s_num_sources].is3D = false;
			return s_num_sources++;
		}
	}
	
	return -1;
}

#endif // USE_OPENAL_AUDIO

/*****************************************************************************
**							   Public Functions								**
*****************************************************************************/

// Initialize OpenAL audio system
void Audio_Init( void )
{
#ifdef USE_OPENAL_AUDIO
	if (s_initialized)
		return;
	
	// Open default audio device
	s_device = alcOpenDevice(NULL);
	if (!s_device)
	{
		printf("OpenAL: Failed to open audio device\n");
		return;
	}
	
	// Create audio context
	s_context = alcCreateContext(s_device, NULL);
	if (!s_context)
	{
		printf("OpenAL: Failed to create audio context\n");
		alcCloseDevice(s_device);
		s_device = NULL;
		return;
	}
	
	// Make context current
	if (!alcMakeContextCurrent(s_context))
	{
		printf("OpenAL: Failed to make context current\n");
		alcDestroyContext(s_context);
		alcCloseDevice(s_device);
		s_context = NULL;
		s_device = NULL;
		return;
	}
	
	// Initialize buffers and sources arrays
	memset(s_buffers, 0, sizeof(s_buffers));
	memset(s_sources, 0, sizeof(s_sources));
	s_num_buffers = 0;
	s_num_sources = 0;
	
	// Generate music source
	alGenSources(1, &s_musicSource);
	CheckALError("generate music source");
	
	// Generate music buffers for streaming
	alGenBuffers(4, s_musicBuffers);
	CheckALError("generate music buffers");
	
	// Set default listener parameters
	ALfloat listenerPos[] = {0.0f, 0.0f, 0.0f};
	ALfloat listenerVel[] = {0.0f, 0.0f, 0.0f};
	ALfloat listenerOri[] = {0.0f, 0.0f, -1.0f, 0.0f, 1.0f, 0.0f};
	
	alListenerfv(AL_POSITION, listenerPos);
	alListenerfv(AL_VELOCITY, listenerVel);
	alListenerfv(AL_ORIENTATION, listenerOri);
	
	// Set distance model for 3D audio
	alDistanceModel(AL_INVERSE_DISTANCE_CLAMPED);
	
	s_initialized = true;
	printf("OpenAL: Audio system initialized\n");
	printf("OpenAL: Vendor: %s\n", alGetString(AL_VENDOR));
	printf("OpenAL: Renderer: %s\n", alGetString(AL_RENDERER));
	printf("OpenAL: Version: %s\n", alGetString(AL_VERSION));
#endif
}

// Deinitialize OpenAL audio system
void Audio_Deinit( void )
{
#ifdef USE_OPENAL_AUDIO
	if (!s_initialized)
		return;
	
	// Stop all sources
	for (int i = 0; i < s_num_sources; i++)
	{
		alSourceStop(s_sources[i].sourceID);
		alDeleteSources(1, &s_sources[i].sourceID);
	}
	
	// Delete music source and buffers
	if (s_musicSource)
	{
		alSourceStop(s_musicSource);
		alDeleteSources(1, &s_musicSource);
		s_musicSource = 0;
	}
	
	alDeleteBuffers(4, s_musicBuffers);
	
	// Delete all sound buffers
	for (int i = 0; i < s_num_buffers; i++)
	{
		if (s_buffers[i].loaded)
		{
			alDeleteBuffers(1, &s_buffers[i].bufferID);
		}
	}
	
	// Destroy context and close device
	alcMakeContextCurrent(NULL);
	if (s_context)
	{
		alcDestroyContext(s_context);
		s_context = NULL;
	}
	if (s_device)
	{
		alcCloseDevice(s_device);
		s_device = NULL;
	}
	
	s_initialized = false;
	s_streaming = false;
	printf("OpenAL: Audio system deinitialized\n");
#endif
}

// Per-frame update
void Audio_Update( void )
{
#ifdef USE_OPENAL_AUDIO
	if (!s_initialized)
		return;
	
	// Update streaming music if needed
	if (s_streaming)
	{
		ALint state;
		alGetSourcei(s_musicSource, AL_SOURCE_STATE, &state);
		if (state != AL_PLAYING && state != AL_PAUSED)
		{
			s_streaming = false;
		}
	}
	
	// Process any pending audio
	alcProcessContext(s_context);
#endif
}

// Check if a music track exists
bool Audio_TrackExists( const char* nameOfFile, int trackNumber )
{
#ifdef USE_OPENAL_AUDIO
	FILE* file = fopen(nameOfFile, "rb");
	if (file)
	{
		fclose(file);
		return true;
	}
	return false;
#else
	return false;
#endif
}

// Load music header (for streaming, would need actual file decoder)
bool Audio_LoadMusicHeader( const char* nameOfFile )
{
#ifdef USE_OPENAL_AUDIO
	if (!s_initialized)
		return false;
	
	// This is a simplified version - real implementation would need
	// a decoder library (libsndfile, libvorbis, etc.) to read audio data
	printf("OpenAL: Music loading requires audio decoder library\n");
	return false;
#else
	return false;
#endif
}

// Preload music stream
bool Audio_PreLoadMusicStream( uint32 checksum )
{
#ifdef USE_OPENAL_AUDIO
	// Would preload initial buffers here
	return false;
#else
	return false;
#endif
}

// Start streaming music
bool Audio_StartStreaming( void )
{
#ifdef USE_OPENAL_AUDIO
	if (!s_initialized)
		return false;
	
	// Set source parameters
	alSourcef(s_musicSource, AL_GAIN, s_music_volume);
	alSourcef(s_musicSource, AL_PITCH, 1.0f);
	alSource3f(s_musicSource, AL_POSITION, 0.0f, 0.0f, 0.0f);
	alSource3f(s_musicSource, AL_VELOCITY, 0.0f, 0.0f, 0.0f);
	alSourcei(s_musicSource, AL_LOOPING, AL_FALSE); // We'll handle looping manually
	
	// Would queue buffers and play here
	// alSourceQueueBuffers(s_musicSource, 4, s_musicBuffers);
	// alSourcePlay(s_musicSource);
	
	s_streaming = true;
	printf("OpenAL: Started streaming (stub - needs decoder)\n");
	return true;
#else
	s_streaming = true;
	return true;
#endif
}

// Stop streaming music
bool Audio_StopStreaming( bool wait_for_last_write )
{
#ifdef USE_OPENAL_AUDIO
	if (!s_initialized)
		return true;
	
	alSourceStop(s_musicSource);
	
	// Unqueue all buffers
	ALint queued;
	alGetSourcei(s_musicSource, AL_BUFFERS_QUEUED, &queued);
	while (queued--)
	{
		ALuint buffer;
		alSourceUnqueueBuffers(s_musicSource, 1, &buffer);
	}
	
	s_streaming = false;
	printf("OpenAL: Stopped streaming\n");
	return true;
#else
	s_streaming = false;
	return true;
#endif
}

// Pause/unpause music stream
bool Audio_PauseStream( bool pause )
{
#ifdef USE_OPENAL_AUDIO
	if (!s_initialized)
		return false;
	
	if (pause)
	{
		alSourcePause(s_musicSource);
	}
	else
	{
		alSourcePlay(s_musicSource);
	}
	CheckALError(pause ? "pause music" : "resume music");
	return true;
#else
	return true;
#endif
}

// Set stream volume
bool Audio_SetVolume( float volume )
{
#ifdef USE_OPENAL_AUDIO
	s_stream_volume = volume;
	if (s_stream_volume < 0.0f) s_stream_volume = 0.0f;
	if (s_stream_volume > 1.0f) s_stream_volume = 1.0f;
	
	if (s_initialized)
	{
		alSourcef(s_musicSource, AL_GAIN, s_stream_volume);
		CheckALError("set volume");
	}
	return true;
#else
	s_stream_volume = volume;
	return true;
#endif
}

// Get stream volume
float Audio_GetVolume( void )
{
	return s_stream_volume;
}

// Set music volume
void Audio_SetMusicVolume( float volume )
{
#ifdef USE_OPENAL_AUDIO
	s_music_volume = volume;
	if (s_music_volume < 0.0f) s_music_volume = 0.0f;
	if (s_music_volume > 1.0f) s_music_volume = 1.0f;
	
	if (s_initialized)
	{
		alSourcef(s_musicSource, AL_GAIN, s_music_volume);
		CheckALError("set music volume");
	}
#else
	s_music_volume = volume;
#endif
}

// Get music volume
float Audio_GetMusicVolume( void )
{
	return s_music_volume;
}

// Load sound effect (simplified - needs audio decoder)
bool LoadSoundEffect( uint32 checksum, const char* filename, bool is3D )
{
#ifdef USE_OPENAL_AUDIO
	if (!s_initialized)
		return false;
	
	// Check if already loaded
	for (int i = 0; i < s_num_buffers; i++)
	{
		if (s_buffers[i].checksum == checksum)
		{
			return true;
		}
	}
	
	// Check capacity
	if (s_num_buffers >= OPENAL_MAX_BUFFERS)
	{
		printf("OpenAL: Buffer limit reached\n");
		return false;
	}
	
	// Generate buffer
	ALuint bufferID;
	alGenBuffers(1, &bufferID);
	CheckALError("generate buffer");
	
	// Would load audio data here using a decoder library
	// For now, just store the metadata
	s_buffers[s_num_buffers].checksum = checksum;
	s_buffers[s_num_buffers].bufferID = bufferID;
	strncpy(s_buffers[s_num_buffers].filename, filename, 255);
	s_buffers[s_num_buffers].loaded = true;
	s_buffers[s_num_buffers].is3D = is3D;
	s_num_buffers++;
	
	printf("OpenAL: Loaded sound effect metadata '%s' (needs decoder for data)\n", filename);
	return true;
#else
	return false;
#endif
}

// Unload sound effect
void UnloadSoundEffect( uint32 checksum )
{
#ifdef USE_OPENAL_AUDIO
	for (int i = 0; i < s_num_buffers; i++)
	{
		if (s_buffers[i].checksum == checksum)
		{
			if (s_buffers[i].loaded)
			{
				alDeleteBuffers(1, &s_buffers[i].bufferID);
			}
			
			// Shift remaining buffers
			for (int j = i; j < s_num_buffers - 1; j++)
			{
				s_buffers[j] = s_buffers[j + 1];
			}
			s_num_buffers--;
			return;
		}
	}
#endif
}

// Play sound effect
int PlaySoundEffect( uint32 checksum, float volume, float pitch )
{
#ifdef USE_OPENAL_AUDIO
	if (!s_initialized)
		return -1;
	
	// Find buffer
	ALuint bufferID = 0;
	bool is3D = false;
	for (int i = 0; i < s_num_buffers; i++)
	{
		if (s_buffers[i].checksum == checksum)
		{
			bufferID = s_buffers[i].bufferID;
			is3D = s_buffers[i].is3D;
			break;
		}
	}
	
	if (bufferID == 0)
	{
		printf("OpenAL: Sound effect not found (checksum: 0x%08X)\n", checksum);
		return -1;
	}
	
	// Get available source
	int sourceIndex = GetAvailableSource();
	if (sourceIndex < 0)
	{
		printf("OpenAL: No available sources\n");
		return -1;
	}
	
	ALuint sourceID = s_sources[sourceIndex].sourceID;
	
	// Configure source
	alSourcei(sourceID, AL_BUFFER, bufferID);
	alSourcef(sourceID, AL_GAIN, volume);
	alSourcef(sourceID, AL_PITCH, pitch);
	alSourcei(sourceID, AL_LOOPING, AL_FALSE);
	
	if (is3D)
	{
		// 3D source - use position
		alSourcei(sourceID, AL_SOURCE_RELATIVE, AL_FALSE);
	}
	else
	{
		// 2D source - relative to listener
		alSourcei(sourceID, AL_SOURCE_RELATIVE, AL_TRUE);
		alSource3f(sourceID, AL_POSITION, 0.0f, 0.0f, 0.0f);
	}
	
	// Play source
	alSourcePlay(sourceID);
	CheckALError("play sound effect");
	
	s_sources[sourceIndex].inUse = true;
	s_sources[sourceIndex].bufferChecksum = checksum;
	s_sources[sourceIndex].is3D = is3D;
	
	return sourceIndex;
#else
	return -1;
#endif
}

// Stop sound effect
void StopSoundEffect( int sourceIndex )
{
#ifdef USE_OPENAL_AUDIO
	if (s_initialized && sourceIndex >= 0 && sourceIndex < s_num_sources)
	{
		alSourceStop(s_sources[sourceIndex].sourceID);
		s_sources[sourceIndex].inUse = false;
	}
#endif
}

// Set sound effect volume
void SetSoundEffectVolume( int sourceIndex, float volume )
{
#ifdef USE_OPENAL_AUDIO
	if (s_initialized && sourceIndex >= 0 && sourceIndex < s_num_sources)
	{
		if (volume < 0.0f) volume = 0.0f;
		if (volume > 1.0f) volume = 1.0f;
		alSourcef(s_sources[sourceIndex].sourceID, AL_GAIN, volume);
	}
#endif
}

// Set sound effect pitch
void SetSoundEffectPitch( int sourceIndex, float pitch )
{
#ifdef USE_OPENAL_AUDIO
	if (s_initialized && sourceIndex >= 0 && sourceIndex < s_num_sources)
	{
		if (pitch < 0.5f) pitch = 0.5f;
		if (pitch > 2.0f) pitch = 2.0f;
		alSourcef(s_sources[sourceIndex].sourceID, AL_PITCH, pitch);
	}
#endif
}

// Check if sound effect is playing
bool IsSoundEffectPlaying( int sourceIndex )
{
#ifdef USE_OPENAL_AUDIO
	if (s_initialized && sourceIndex >= 0 && sourceIndex < s_num_sources)
	{
		ALint state;
		alGetSourcei(s_sources[sourceIndex].sourceID, AL_SOURCE_STATE, &state);
		return (state == AL_PLAYING);
	}
#endif
	return false;
}

// Set 3D listener position
void Set3DListenerPosition( float x, float y, float z )
{
#ifdef USE_OPENAL_AUDIO
	if (s_initialized)
	{
		alListener3f(AL_POSITION, x, y, z);
		CheckALError("set listener position");
	}
#endif
}

// Set 3D listener velocity (for Doppler effect)
void Set3DListenerVelocity( float vx, float vy, float vz )
{
#ifdef USE_OPENAL_AUDIO
	if (s_initialized)
	{
		alListener3f(AL_VELOCITY, vx, vy, vz);
		CheckALError("set listener velocity");
	}
#endif
}

// Set 3D listener orientation
void Set3DListenerOrientation( float forwardX, float forwardY, float forwardZ,
								float upX, float upY, float upZ )
{
#ifdef USE_OPENAL_AUDIO
	if (s_initialized)
	{
		ALfloat orientation[] = {forwardX, forwardY, forwardZ, upX, upY, upZ};
		alListenerfv(AL_ORIENTATION, orientation);
		CheckALError("set listener orientation");
	}
#endif
}

// Set 3D sound position
void Set3DSoundPosition( int sourceIndex, float x, float y, float z )
{
#ifdef USE_OPENAL_AUDIO
	if (s_initialized && sourceIndex >= 0 && sourceIndex < s_num_sources)
	{
		alSource3f(s_sources[sourceIndex].sourceID, AL_POSITION, x, y, z);
		CheckALError("set source position");
	}
#endif
}

// Set 3D sound velocity (for Doppler effect)
void Set3DSoundVelocity( int sourceIndex, float vx, float vy, float vz )
{
#ifdef USE_OPENAL_AUDIO
	if (s_initialized && sourceIndex >= 0 && sourceIndex < s_num_sources)
	{
		alSource3f(s_sources[sourceIndex].sourceID, AL_VELOCITY, vx, vy, vz);
		CheckALError("set source velocity");
	}
#endif
}

// Set 3D sound attenuation
void Set3DSoundAttenuation( int sourceIndex, float rolloffFactor, float referenceDistance, float maxDistance )
{
#ifdef USE_OPENAL_AUDIO
	if (s_initialized && sourceIndex >= 0 && sourceIndex < s_num_sources)
	{
		ALuint sourceID = s_sources[sourceIndex].sourceID;
		alSourcef(sourceID, AL_ROLLOFF_FACTOR, rolloffFactor);
		alSourcef(sourceID, AL_REFERENCE_DISTANCE, referenceDistance);
		alSourcef(sourceID, AL_MAX_DISTANCE, maxDistance);
		CheckALError("set source attenuation");
	}
#endif
}

// Set 3D sound cone (directional audio)
void Set3DSoundCone( int sourceIndex, float innerAngle, float outerAngle, float outerGain )
{
#ifdef USE_OPENAL_AUDIO
	if (s_initialized && sourceIndex >= 0 && sourceIndex < s_num_sources)
	{
		ALuint sourceID = s_sources[sourceIndex].sourceID;
		alSourcef(sourceID, AL_CONE_INNER_ANGLE, innerAngle);
		alSourcef(sourceID, AL_CONE_OUTER_ANGLE, outerAngle);
		alSourcef(sourceID, AL_CONE_OUTER_GAIN, outerGain);
		CheckALError("set source cone");
	}
#endif
}

// Set Doppler factor
void SetDopplerFactor( float factor )
{
#ifdef USE_OPENAL_AUDIO
	if (s_initialized)
	{
		alDopplerFactor(factor);
		CheckALError("set doppler factor");
	}
#endif
}

// Set speed of sound
void SetSpeedOfSound( float speed )
{
#ifdef USE_OPENAL_AUDIO
	if (s_initialized)
	{
		alSpeedOfSound(speed);
		CheckALError("set speed of sound");
	}
#endif
}

} // namespace OpenAL
} // namespace Pcm

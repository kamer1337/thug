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
**	Module:			Music (Pcm) - SDL2_mixer Backend						**
**																			**
**	File name:		p_audio.cpp												**
**																			**
**	Created by:		PC Port													**
**																			**
**	Description:	SDL2_mixer audio backend implementation					**
**																			**
*****************************************************************************/

/*****************************************************************************
**							  	  Includes									**
*****************************************************************************/

#include <gel/music/SDL2/p_audio.h>
#include <core/defines.h>

#ifdef USE_SDL2_AUDIO
#include <SDL2/SDL.h>
#include <SDL2/SDL_mixer.h>
#include <string.h>
#include <stdio.h>
#include <math.h>
#endif

/*****************************************************************************
**								   Defines									**
*****************************************************************************/

namespace Pcm
{
namespace SDL2
{

#ifdef USE_SDL2_AUDIO

// Internal structures for sound management
struct SoundEffect
{
	uint32 checksum;
	Mix_Chunk* chunk;
	char filename[256];
	bool loaded;
};

struct StreamInfo
{
	Mix_Music* music;
	char filename[256];
	bool loaded;
	bool playing;
};

// Internal state
static bool s_initialized = false;
static bool s_streaming = false;
static float s_stream_volume = 1.0f;
static float s_music_volume = 1.0f;

// Sound effects management
static SoundEffect s_sound_effects[256];
static int s_num_sound_effects = 0;

// Music streaming
static StreamInfo s_stream_info;

// 3D audio state
struct Listener3D
{
	float x, y, z;
	float forwardX, forwardY, forwardZ;
	float upX, upY, upZ;
};

static Listener3D s_listener;

// Helper function to calculate 3D volume and panning
// This implements a simple distance-based attenuation with stereo panning
static void Calculate3DAudio(float sourceX, float sourceY, float sourceZ,
							 float minDist, float maxDist,
							 float& outVolume, float& outPanning)
{
	// Calculate Euclidean distance from listener to source
	float dx = sourceX - s_listener.x;
	float dy = sourceY - s_listener.y;
	float dz = sourceZ - s_listener.z;
	float distance = sqrtf(dx*dx + dy*dy + dz*dz);
	
	// Calculate volume based on distance (linear falloff model)
	// Volume is 1.0 (full) within minDist, 0.0 (silent) beyond maxDist
	if (distance < minDist)
	{
		outVolume = 1.0f;
	}
	else if (distance > maxDist)
	{
		outVolume = 0.0f;
	}
	else
	{
		// Linear falloff between minDist and maxDist
		outVolume = 1.0f - ((distance - minDist) / (maxDist - minDist));
	}
	
	// Calculate stereo panning based on left/right position relative to listener
	// First, compute the listener's right vector (cross product of forward and up)
	float rightX = s_listener.forwardY * s_listener.upZ - s_listener.forwardZ * s_listener.upY;
	float rightZ = s_listener.forwardX * s_listener.upY - s_listener.forwardY * s_listener.upX;
	
	// Normalize right vector
	float rightLen = sqrtf(rightX*rightX + rightZ*rightZ);
	if (rightLen > 0.0f)
	{
		rightX /= rightLen;
		rightZ /= rightLen;
	}
	
	// Dot product of source direction with right vector gives left/right position
	float dot = dx * rightX + dz * rightZ;
	outPanning = dot / (distance + 0.001f); // Normalize by distance, avoid div by zero
	outPanning = (outPanning + 1.0f) * 0.5f; // Map from [-1,1] to [0,1] where 0=left, 1=right
	
	// Clamp values to valid range
	if (outVolume < 0.0f) outVolume = 0.0f;
	if (outVolume > 1.0f) outVolume = 1.0f;
	if (outPanning < 0.0f) outPanning = 0.0f;
	if (outPanning > 1.0f) outPanning = 1.0f;
}

#endif // USE_SDL2_AUDIO

/*****************************************************************************
**							   Public Functions								**
*****************************************************************************/

// Initialize SDL2_mixer audio system
void Audio_Init( void )
{
#ifdef USE_SDL2_AUDIO
	if (s_initialized)
		return;
	
	// Initialize SDL audio subsystem
	if (SDL_InitSubSystem(SDL_INIT_AUDIO) < 0)
	{
		printf("SDL2_mixer: Failed to initialize SDL audio: %s\n", SDL_GetError());
		return;
	}
	
	// Open audio device with high quality settings
	if (Mix_OpenAudio(SDL2_AUDIO_FREQUENCY, SDL2_AUDIO_FORMAT, SDL2_AUDIO_CHANNELS, SDL2_AUDIO_CHUNKSIZE) < 0)
	{
		printf("SDL2_mixer: Failed to open audio device: %s\n", Mix_GetError());
		SDL_QuitSubSystem(SDL_INIT_AUDIO);
		return;
	}
	
	// Allocate mixing channels for sound effects
	Mix_AllocateChannels(SDL2_MAX_SOUND_CHANNELS);
	
	// Initialize sound effects array
	memset(s_sound_effects, 0, sizeof(s_sound_effects));
	s_num_sound_effects = 0;
	
	// Initialize stream info
	memset(&s_stream_info, 0, sizeof(s_stream_info));
	
	// Initialize 3D audio listener at origin
	s_listener.x = 0.0f;
	s_listener.y = 0.0f;
	s_listener.z = 0.0f;
	s_listener.forwardX = 0.0f;
	s_listener.forwardY = 0.0f;
	s_listener.forwardZ = 1.0f;
	s_listener.upX = 0.0f;
	s_listener.upY = 1.0f;
	s_listener.upZ = 0.0f;
	
	s_initialized = true;
	printf("SDL2_mixer: Audio system initialized\n");
#endif
}

// Deinitialize SDL2_mixer audio system
void Audio_Deinit( void )
{
#ifdef USE_SDL2_AUDIO
	if (!s_initialized)
		return;
	
	// Stop all audio
	Mix_HaltMusic();
	Mix_HaltChannel(-1);
	
	// Free all loaded sound effects
	for (int i = 0; i < s_num_sound_effects; i++)
	{
		if (s_sound_effects[i].chunk)
		{
			Mix_FreeChunk(s_sound_effects[i].chunk);
			s_sound_effects[i].chunk = NULL;
		}
	}
	
	// Free music
	if (s_stream_info.music)
	{
		Mix_FreeMusic(s_stream_info.music);
		s_stream_info.music = NULL;
	}
	
	// Close audio device
	Mix_CloseAudio();
	
	// Quit SDL audio subsystem
	SDL_QuitSubSystem(SDL_INIT_AUDIO);
	
	s_initialized = false;
	s_streaming = false;
	printf("SDL2_mixer: Audio system deinitialized\n");
#endif
}

// Per-frame update
void Audio_Update( void )
{
#ifdef USE_SDL2_AUDIO
	// SDL2_mixer handles mixing automatically
	// This function can be used for additional per-frame logic if needed
	
	// Check if music has finished playing
	if (s_streaming && !Mix_PlayingMusic())
	{
		s_streaming = false;
	}
#endif
}

// Check if a music track exists
bool Audio_TrackExists( const char* nameOfFile, int trackNumber )
{
#ifdef USE_SDL2_AUDIO
	// Simple file existence check
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

// Load music header
bool Audio_LoadMusicHeader( const char* nameOfFile )
{
#ifdef USE_SDL2_AUDIO
	if (!s_initialized)
		return false;
	
	// Free previous music if loaded
	if (s_stream_info.music)
	{
		Mix_FreeMusic(s_stream_info.music);
		s_stream_info.music = NULL;
	}
	
	// Load music file
	s_stream_info.music = Mix_LoadMUS(nameOfFile);
	if (!s_stream_info.music)
	{
		printf("SDL2_mixer: Failed to load music '%s': %s\n", nameOfFile, Mix_GetError());
		return false;
	}
	
	strncpy(s_stream_info.filename, nameOfFile, sizeof(s_stream_info.filename) - 1);
	s_stream_info.loaded = true;
	s_stream_info.playing = false;
	
	printf("SDL2_mixer: Loaded music '%s'\n", nameOfFile);
	return true;
#else
	return false;
#endif
}

// Preload music stream
bool Audio_PreLoadMusicStream( uint32 checksum )
{
#ifdef USE_SDL2_AUDIO
	// For SDL2_mixer, music is already loaded in LoadMusicHeader
	// This function confirms the music is ready to play
	return s_stream_info.loaded;
#else
	return false;
#endif
}

// Start streaming music
bool Audio_StartStreaming( void )
{
#ifdef USE_SDL2_AUDIO
	if (!s_initialized || !s_stream_info.loaded || !s_stream_info.music)
		return false;
	
	// Set volume before playing
	Mix_VolumeMusic((int)(s_music_volume * MIX_MAX_VOLUME));
	
	// Start playing music (loop forever: -1)
	if (Mix_PlayMusic(s_stream_info.music, -1) == -1)
	{
		printf("SDL2_mixer: Failed to play music: %s\n", Mix_GetError());
		return false;
	}
	
	s_streaming = true;
	s_stream_info.playing = true;
	printf("SDL2_mixer: Started streaming music\n");
	return true;
#else
	s_streaming = true;
	return true;
#endif
}

// Stop streaming music
bool Audio_StopStreaming( bool wait_for_last_write )
{
#ifdef USE_SDL2_AUDIO
	if (!s_initialized)
		return true;
	
	if (wait_for_last_write)
	{
		// Fade out over 500ms
		Mix_FadeOutMusic(500);
	}
	else
	{
		// Stop immediately
		Mix_HaltMusic();
	}
	
	s_streaming = false;
	s_stream_info.playing = false;
	printf("SDL2_mixer: Stopped streaming music\n");
	return true;
#else
	s_streaming = false;
	return true;
#endif
}

// Pause/unpause music stream
bool Audio_PauseStream( bool pause )
{
#ifdef USE_SDL2_AUDIO
	if (!s_initialized)
		return false;
	
	if (pause)
	{
		Mix_PauseMusic();
		printf("SDL2_mixer: Paused music\n");
	}
	else
	{
		Mix_ResumeMusic();
		printf("SDL2_mixer: Resumed music\n");
	}
	return true;
#else
	return true;
#endif
}

// Set stream volume
bool Audio_SetVolume( float volume )
{
#ifdef USE_SDL2_AUDIO
	s_stream_volume = volume;
	if (volume < 0.0f) s_stream_volume = 0.0f;
	if (volume > 1.0f) s_stream_volume = 1.0f;
	
	if (s_initialized)
	{
		Mix_VolumeMusic((int)(s_stream_volume * MIX_MAX_VOLUME));
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
#ifdef USE_SDL2_AUDIO
	s_music_volume = volume;
	if (s_music_volume < 0.0f) s_music_volume = 0.0f;
	if (s_music_volume > 1.0f) s_music_volume = 1.0f;
	
	if (s_initialized)
	{
		Mix_VolumeMusic((int)(s_music_volume * MIX_MAX_VOLUME));
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

// Load sound effect
bool LoadSoundEffect( uint32 checksum, const char* filename )
{
#ifdef USE_SDL2_AUDIO
	if (!s_initialized)
		return false;
	
	// Check if already loaded
	for (int i = 0; i < s_num_sound_effects; i++)
	{
		if (s_sound_effects[i].checksum == checksum)
		{
			return true; // Already loaded
		}
	}
	
	// Check capacity
	if (s_num_sound_effects >= 256)
	{
		printf("SDL2_mixer: Sound effect limit reached\n");
		return false;
	}
	
	// Load sound effect
	Mix_Chunk* chunk = Mix_LoadWAV(filename);
	if (!chunk)
	{
		printf("SDL2_mixer: Failed to load sound effect '%s': %s\n", filename, Mix_GetError());
		return false;
	}
	
	// Store sound effect
	s_sound_effects[s_num_sound_effects].checksum = checksum;
	s_sound_effects[s_num_sound_effects].chunk = chunk;
	strncpy(s_sound_effects[s_num_sound_effects].filename, filename, sizeof(s_sound_effects[s_num_sound_effects].filename) - 1);
	s_sound_effects[s_num_sound_effects].filename[sizeof(s_sound_effects[s_num_sound_effects].filename) - 1] = '\0';
	s_sound_effects[s_num_sound_effects].loaded = true;
	s_num_sound_effects++;
	
	printf("SDL2_mixer: Loaded sound effect '%s' (checksum: 0x%08X)\n", filename, checksum);
	return true;
#else
	return false;
#endif
}

// Unload sound effect
void UnloadSoundEffect( uint32 checksum )
{
#ifdef USE_SDL2_AUDIO
	for (int i = 0; i < s_num_sound_effects; i++)
	{
		if (s_sound_effects[i].checksum == checksum)
		{
			if (s_sound_effects[i].chunk)
			{
				Mix_FreeChunk(s_sound_effects[i].chunk);
			}
			
			// Shift remaining effects down
			for (int j = i; j < s_num_sound_effects - 1; j++)
			{
				s_sound_effects[j] = s_sound_effects[j + 1];
			}
			s_num_sound_effects--;
			return;
		}
	}
#endif
}

// Play sound effect
int PlaySoundEffect( uint32 checksum, float volume, float pitch )
{
#ifdef USE_SDL2_AUDIO
	if (!s_initialized)
		return -1;
	
	// Find sound effect
	Mix_Chunk* chunk = NULL;
	for (int i = 0; i < s_num_sound_effects; i++)
	{
		if (s_sound_effects[i].checksum == checksum)
		{
			chunk = s_sound_effects[i].chunk;
			break;
		}
	}
	
	if (!chunk)
	{
		printf("SDL2_mixer: Sound effect not found (checksum: 0x%08X)\n", checksum);
		return -1;
	}
	
	// Play on first available channel (0 means play once)
	int channel = Mix_PlayChannel(-1, chunk, 0);
	if (channel == -1)
	{
		printf("SDL2_mixer: Failed to play sound effect: %s\n", Mix_GetError());
		return -1;
	}
	
	// Set volume
	Mix_Volume(channel, (int)(volume * MIX_MAX_VOLUME));
	
	// Note: SDL2_mixer doesn't support pitch shifting directly
	// Would need SDL2_sound or custom DSP for pitch control
	
	return channel;
#else
	return -1;
#endif
}

// Stop sound effect
void StopSoundEffect( int channel )
{
#ifdef USE_SDL2_AUDIO
	if (s_initialized && channel >= 0)
	{
		Mix_HaltChannel(channel);
	}
#endif
}

// Set sound effect volume
void SetSoundEffectVolume( int channel, float volume )
{
#ifdef USE_SDL2_AUDIO
	if (s_initialized && channel >= 0)
	{
		if (volume < 0.0f) volume = 0.0f;
		if (volume > 1.0f) volume = 1.0f;
		Mix_Volume(channel, (int)(volume * MIX_MAX_VOLUME));
	}
#endif
}

// Check if sound effect is playing
bool IsSoundEffectPlaying( int channel )
{
#ifdef USE_SDL2_AUDIO
	if (s_initialized && channel >= 0)
	{
		return Mix_Playing(channel) != 0;
	}
#endif
	return false;
}

// Set 3D listener position
void Set3DListenerPosition( float x, float y, float z )
{
#ifdef USE_SDL2_AUDIO
	s_listener.x = x;
	s_listener.y = y;
	s_listener.z = z;
#endif
}

// Set 3D listener orientation
void Set3DListenerOrientation( float forwardX, float forwardY, float forwardZ,
								float upX, float upY, float upZ )
{
#ifdef USE_SDL2_AUDIO
	s_listener.forwardX = forwardX;
	s_listener.forwardY = forwardY;
	s_listener.forwardZ = forwardZ;
	s_listener.upX = upX;
	s_listener.upY = upY;
	s_listener.upZ = upZ;
#endif
}

// Set 3D sound position
void Set3DSoundPosition( int channel, float x, float y, float z )
{
#ifdef USE_SDL2_AUDIO
	if (!s_initialized || channel < 0)
		return;
	
	// Calculate 3D audio parameters
	float volume, panning;
	Calculate3DAudio(x, y, z, 10.0f, 100.0f, volume, panning);
	
	// Apply volume
	Mix_Volume(channel, (int)(volume * MIX_MAX_VOLUME));
	
	// Apply panning (left channel, right channel)
	Uint8 left = (Uint8)((1.0f - panning) * 255.0f);
	Uint8 right = (Uint8)(panning * 255.0f);
	Mix_SetPanning(channel, left, right);
#endif
}

// Set 3D sound attenuation
void Set3DSoundAttenuation( int channel, float minDist, float maxDist )
{
#ifdef USE_SDL2_AUDIO
	// SDL2_mixer doesn't have built-in distance attenuation
	// This would need to be implemented with custom distance calculation
	// and manual volume adjustment using Set3DSoundPosition
#endif
}

} // namespace SDL2
} // namespace Pcm

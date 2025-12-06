/**
 * Audio and Video System Examples
 * 
 * These examples demonstrate how to use the audio and video playback
 * systems in the THUG engine.
 */

#include <stdio.h>

// Audio includes (choose one backend)
#ifdef USE_SDL2_AUDIO
#include <gel/music/SDL2/p_audio.h>
#endif

#ifdef USE_OPENAL_AUDIO
#include <gel/music/OpenAL/p_audio.h>
#endif

#ifdef USE_FMOD_AUDIO
#include <gel/music/FMOD/p_audio.h>
#endif

// Video include
#ifdef USE_FFMPEG_VIDEO
#include <gel/Movies/FFmpeg/p_videoplayer.h>
#endif

// Asset loader
#include <gel/AssetLoader/AssetLoader.h>

/**
 * Example 1: Play background music with SDL2_mixer
 */
void Example_PlayBackgroundMusic()
{
#ifdef USE_SDL2_AUDIO
	printf("=== Example 1: Background Music ===\n");
	
	// Initialize audio system
	Pcm::SDL2::Audio_Init();
	
	// Load music file
	const char* musicFile = "Data/Audio/Music/menu_music.ogg";
	if (Pcm::SDL2::Audio_LoadMusicHeader(musicFile))
	{
		printf("Loaded music: %s\n", musicFile);
		
		// Set volume (0.0 to 1.0)
		Pcm::SDL2::Audio_SetMusicVolume(0.7f);
		
		// Start playing
		Pcm::SDL2::Audio_StartStreaming();
		printf("Playing music...\n");
		
		// In game loop, call:
		// Pcm::SDL2::Audio_Update();
		
		// To stop:
		// Pcm::SDL2::Audio_StopStreaming();
	}
	else
	{
		printf("Failed to load music: %s\n", musicFile);
	}
	
	// Don't deinit yet - music is playing
	// Pcm::SDL2::Audio_Deinit();
#else
	printf("SDL2 audio not compiled in\n");
#endif
}

/**
 * Example 2: Play sound effects
 */
void Example_PlaySoundEffects()
{
#ifdef USE_SDL2_AUDIO
	printf("\n=== Example 2: Sound Effects ===\n");
	
	// Initialize (if not already done)
	Pcm::SDL2::Audio_Init();
	
	// Load sound effects
	// Note: In production code, use proper checksum calculation from filename
	// These are placeholder values for demonstration purposes
	uint32 jumpSoundID = 0x12345678;
	uint32 landSoundID = 0x87654321;
	
	if (Pcm::SDL2::LoadSoundEffect(jumpSoundID, "Data/Audio/SFX/jump.wav"))
	{
		printf("Loaded jump sound\n");
	}
	
	if (Pcm::SDL2::LoadSoundEffect(landSoundID, "Data/Audio/SFX/land.wav"))
	{
		printf("Loaded land sound\n");
	}
	
	// Play sound effects
	int jumpChannel = Pcm::SDL2::PlaySoundEffect(jumpSoundID, 1.0f, 1.0f);
	printf("Playing jump sound on channel %d\n", jumpChannel);
	
	// Later, play land sound at lower volume
	int landChannel = Pcm::SDL2::PlaySoundEffect(landSoundID, 0.5f, 1.0f);
	printf("Playing land sound on channel %d\n", landChannel);
	
	// Check if sound is still playing
	if (Pcm::SDL2::IsSoundEffectPlaying(jumpChannel))
	{
		// Adjust volume mid-playback
		Pcm::SDL2::SetSoundEffectVolume(jumpChannel, 0.3f);
	}
	
	// Cleanup
	Pcm::SDL2::UnloadSoundEffect(jumpSoundID);
	Pcm::SDL2::UnloadSoundEffect(landSoundID);
#else
	printf("SDL2 audio not compiled in\n");
#endif
}

/**
 * Example 3: 3D positional audio with OpenAL
 */
void Example_3DPositionalAudio()
{
#ifdef USE_OPENAL_AUDIO
	printf("\n=== Example 3: 3D Positional Audio ===\n");
	
	// Initialize OpenAL
	Pcm::OpenAL::Audio_Init();
	
	// Set listener position (player/camera)
	float playerX = 0.0f, playerY = 0.0f, playerZ = 0.0f;
	Pcm::OpenAL::Set3DListenerPosition(playerX, playerY, playerZ);
	
	// Set listener orientation
	Pcm::OpenAL::Set3DListenerOrientation(
		0.0f, 0.0f, -1.0f,  // forward vector
		0.0f, 1.0f, 0.0f    // up vector
	);
	
	// Load a 3D sound (engine sound)
	uint32 engineSoundID = 0xABCDEF12;
	if (Pcm::OpenAL::LoadSoundEffect(engineSoundID, "Data/Audio/SFX/engine_idle.wav", true))
	{
		printf("Loaded 3D engine sound\n");
		
		// Play sound
		int sourceID = Pcm::OpenAL::PlaySoundEffect(engineSoundID, 1.0f, 1.0f);
		
		// Position sound in 3D space (e.g., car position)
		float carX = 10.0f, carY = 0.0f, carZ = 20.0f;
		Pcm::OpenAL::Set3DSoundPosition(sourceID, carX, carY, carZ);
		
		// Set attenuation (how sound fades with distance)
		Pcm::OpenAL::Set3DSoundAttenuation(
			sourceID,
			1.0f,   // rolloff factor (higher = faster falloff)
			10.0f,  // reference distance (full volume until this distance)
			100.0f  // max distance (silent beyond this)
		);
		
		// In game loop, update listener position as player moves
		// Pcm::OpenAL::Set3DListenerPosition(newPlayerX, newPlayerY, newPlayerZ);
		// Pcm::OpenAL::Audio_Update();
		
		printf("3D sound playing at position (%.1f, %.1f, %.1f)\n", carX, carY, carZ);
	}
	
	// Cleanup
	Pcm::OpenAL::Audio_Deinit();
#else
	printf("OpenAL audio not compiled in\n");
#endif
}

/**
 * Example 4: Play video cutscene with FFmpeg
 */
void Example_PlayVideoCutscene()
{
#ifdef USE_FFMPEG_VIDEO
	printf("\n=== Example 4: Video Cutscene ===\n");
	
	// Initialize video player
	Flx::FFmpeg::VideoPlayer_Init();
	
	// Load video
	const char* videoFile = "Data/Movies/intro.mp4";
	if (Flx::FFmpeg::VideoPlayer_Load(videoFile))
	{
		// Get video info
		const Flx::FFmpeg::VideoInfo* info = Flx::FFmpeg::VideoPlayer_GetInfo();
		printf("Video loaded: %s\n", videoFile);
		printf("  Resolution: %dx%d\n", info->width, info->height);
		printf("  Frame rate: %.2f fps\n", info->frameRate);
		printf("  Duration: %.2f seconds\n", info->duration);
		printf("  Has audio: %s\n", info->hasAudio ? "Yes" : "No");
		
		// Set volume
		Flx::FFmpeg::VideoPlayer_SetVolume(0.8f);
		
		// Play video (with loop flag)
		if (Flx::FFmpeg::VideoPlayer_Play(Flx::FFmpeg::VIDEO_FLAG_LOOP))
		{
			printf("Playing video...\n");
			
			// In game loop:
			// float deltaTime = GetFrameDeltaTime();
			// if (Flx::FFmpeg::VideoPlayer_Update(deltaTime))
			// {
			//     const Flx::FFmpeg::VideoFrame* frame = Flx::FFmpeg::VideoPlayer_GetCurrentFrame();
			//     // Render frame->data (RGBA format) to screen/texture
			// }
			
			// To pause/resume:
			// Flx::FFmpeg::VideoPlayer_Pause();
			// Flx::FFmpeg::VideoPlayer_Resume();
			
			// To seek to specific time:
			// Flx::FFmpeg::VideoPlayer_Seek(30.0f); // 30 seconds
		}
		
		// Stop when done
		Flx::FFmpeg::VideoPlayer_Stop();
	}
	else
	{
		printf("Failed to load video: %s\n", videoFile);
	}
	
	// Cleanup
	Flx::FFmpeg::VideoPlayer_Deinit();
#else
	printf("FFmpeg video not compiled in\n");
#endif
}

/**
 * Example 5: Using the Asset Loader
 */
void Example_AssetLoader()
{
	printf("\n=== Example 5: Asset Loader ===\n");
	
	// Get singleton instance
	Asset::CAssetLoader* loader = Asset::CAssetLoader::Instance();
	
	// Initialize
	loader->Init();
	
	// Load audio asset
	Asset::AssetData* musicAsset = loader->LoadAsset(
		"Data/Audio/Music/game_music.ogg",
		Asset::ASSET_TYPE_AUDIO,
		Asset::ASSET_LOAD_CACHED | Asset::ASSET_LOAD_PRELOAD
	);
	
	if (musicAsset && musicAsset->status == Asset::ASSET_STATUS_LOADED)
	{
		printf("Audio asset loaded: %s (%zu bytes)\n", 
			   musicAsset->filename, musicAsset->dataSize);
	}
	
	// Load video asset with streaming
	Asset::AssetData* videoAsset = loader->LoadAsset(
		"Data/Movies/replay.mp4",
		Asset::ASSET_TYPE_VIDEO,
		Asset::ASSET_LOAD_STREAMING
	);
	
	if (videoAsset && videoAsset->status == Asset::ASSET_STATUS_LOADED)
	{
		printf("Video asset loaded: %s (%zu bytes)\n",
			   videoAsset->filename, videoAsset->dataSize);
	}
	
	// Check if asset is loaded
	if (loader->IsAssetLoaded("Data/Audio/Music/game_music.ogg"))
	{
		printf("Music asset is in memory\n");
	}
	
	// Get asset by filename
	Asset::AssetData* retrievedAsset = loader->GetAsset("Data/Audio/Music/game_music.ogg");
	if (retrievedAsset)
	{
		printf("Retrieved asset, ref count: %d\n", retrievedAsset->refCount);
	}
	
	// Unload assets
	loader->UnloadAsset(musicAsset);
	loader->UnloadAsset(videoAsset);
	
	// Print statistics
	loader->PrintStatistics();
	
	// Cleanup
	loader->Deinit();
}

/**
 * Example 6: Complete audio/video system integration
 */
void Example_CompleteIntegration()
{
	printf("\n=== Example 6: Complete Integration ===\n");
	
	// Initialize all systems
	Asset::CAssetLoader* loader = Asset::CAssetLoader::Instance();
	loader->Init();
	
#ifdef USE_SDL2_AUDIO
	Pcm::SDL2::Audio_Init();
#endif

#ifdef USE_FFMPEG_VIDEO
	Flx::FFmpeg::VideoPlayer_Init();
#endif
	
	// Load and play intro video with sound
#ifdef USE_FFMPEG_VIDEO
	if (Flx::FFmpeg::VideoPlayer_Load("Data/Movies/intro.mp4"))
	{
		Flx::FFmpeg::VideoPlayer_Play(0); // No loop
		
		// Game loop would go here
		printf("Video playing (simulated)...\n");
		
		Flx::FFmpeg::VideoPlayer_Stop();
	}
#endif
	
	// Load and play menu music
#ifdef USE_SDL2_AUDIO
	if (Pcm::SDL2::Audio_LoadMusicHeader("Data/Audio/Music/menu.ogg"))
	{
		Pcm::SDL2::Audio_StartStreaming();
		printf("Menu music playing...\n");
	}
	
	// Load gameplay sound effects
	uint32 trickSoundID = 0x11111111;
	if (Pcm::SDL2::LoadSoundEffect(trickSoundID, "Data/Audio/SFX/trick.wav"))
	{
		// Play when trick is performed
		Pcm::SDL2::PlaySoundEffect(trickSoundID, 1.0f, 1.0f);
		printf("Trick sound played\n");
	}
#endif
	
	// Cleanup all systems
#ifdef USE_SDL2_AUDIO
	Pcm::SDL2::Audio_StopStreaming();
	Pcm::SDL2::UnloadSoundEffect(trickSoundID);
	Pcm::SDL2::Audio_Deinit();
#endif

#ifdef USE_FFMPEG_VIDEO
	Flx::FFmpeg::VideoPlayer_Deinit();
#endif
	
	loader->UnloadAllAssets(true);
	loader->Deinit();
	
	printf("All systems cleaned up\n");
}

/**
 * Main function - runs all examples
 */
int main(int argc, char* argv[])
{
	printf("===========================================\n");
	printf("THUG Audio/Video System Examples\n");
	printf("===========================================\n\n");
	
	// Run examples
	Example_PlayBackgroundMusic();
	Example_PlaySoundEffects();
	Example_3DPositionalAudio();
	Example_PlayVideoCutscene();
	Example_AssetLoader();
	Example_CompleteIntegration();
	
	printf("\n===========================================\n");
	printf("Examples completed\n");
	printf("===========================================\n");
	
	return 0;
}

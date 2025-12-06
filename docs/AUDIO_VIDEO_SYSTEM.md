# Audio and Video System Documentation

This document describes the audio and video playback systems implemented for the THUG PC port.

## Overview

The THUG engine now includes comprehensive audio and video playback capabilities through multiple backend options:

### Audio Backends
- **SDL2_mixer** - Simple, cross-platform audio with good format support
- **OpenAL** - Professional 3D positional audio with advanced features
- **FMOD** - Commercial-grade audio engine with DSP effects

### Video Backend
- **FFmpeg** - Industry-standard video decoder supporting many formats

### Asset Loader
- **Unified Asset Loader** - Centralized system for loading audio and video files with caching and reference counting

## Audio System

### Supported Audio Formats

| Backend | WAV | OGG | MP3 | FLAC |
|---------|-----|-----|-----|------|
| SDL2_mixer | ✅ | ✅ | ✅ | ❌ |
| OpenAL | ✅* | ✅* | ✅* | ✅* |
| FMOD | ✅ | ✅ | ✅ | ✅ |

*Requires external decoder library (libsndfile, libvorbis, etc.)

### Features by Backend

#### SDL2_mixer
- ✅ Multi-channel mixing (32 channels by default)
- ✅ Music streaming
- ✅ Sound effects
- ✅ Volume control
- ✅ Basic 3D audio (manual panning/volume)
- ❌ Pitch shifting
- ❌ Advanced 3D features

#### OpenAL
- ✅ Multi-source audio (32 sources by default)
- ✅ Music streaming
- ✅ Sound effects
- ✅ Full 3D positional audio
- ✅ Distance attenuation
- ✅ Doppler effect
- ✅ Sound cones (directional audio)
- ✅ Pitch control
- ❌ DSP effects (requires extensions)

#### FMOD
- ✅ Unlimited channels (limited by license)
- ✅ Music streaming
- ✅ Sound effects
- ✅ Full 3D positional audio
- ✅ Distance attenuation
- ✅ Doppler effect
- ✅ DSP effects (reverb, filters, etc.)
- ✅ Pitch control
- ✅ Professional features

### Building with Audio Support

#### SDL2_mixer (Recommended)

```bash
# Linux
sudo apt install libsdl2-dev libsdl2-mixer-dev

# macOS
brew install sdl2 sdl2_mixer

# Build
mkdir build && cd build
cmake -DAUDIO_BACKEND=SDL2 ..
make
```

#### OpenAL

```bash
# Linux
sudo apt install libopenal-dev

# macOS
brew install openal-soft

# Build
mkdir build && cd build
cmake -DAUDIO_BACKEND=OpenAL ..
make
```

#### FMOD

1. Download FMOD SDK from https://www.fmod.com/download
2. Extract to a directory (e.g., `/opt/fmod` or `C:\fmod`)
3. Build:

```bash
mkdir build && cd build
cmake -DAUDIO_BACKEND=FMOD -DFMOD_ROOT=/path/to/fmod ..
make
```

**Note:** FMOD requires a commercial license for released games.

### Usage Examples

#### SDL2 Music Playback

```cpp
#include <gel/music/SDL2/p_audio.h>

// Initialize
Pcm::SDL2::Audio_Init();

// Load music
Pcm::SDL2::Audio_LoadMusicHeader("Data/Audio/Music/track01.ogg");

// Play
Pcm::SDL2::Audio_StartStreaming();

// Set volume (0.0 to 1.0)
Pcm::SDL2::Audio_SetVolume(0.8f);

// Update each frame
Pcm::SDL2::Audio_Update();

// Stop
Pcm::SDL2::Audio_StopStreaming();

// Cleanup
Pcm::SDL2::Audio_Deinit();
```

#### SDL2 Sound Effects

```cpp
// Load sound effect
uint32 checksum = 0x12345678; // Or calculate from filename
Pcm::SDL2::LoadSoundEffect(checksum, "Data/Audio/SFX/jump.wav");

// Play sound
int channel = Pcm::SDL2::PlaySoundEffect(checksum, 1.0f, 1.0f);

// Check if still playing
if (Pcm::SDL2::IsSoundEffectPlaying(channel)) {
    // Adjust volume
    Pcm::SDL2::SetSoundEffectVolume(channel, 0.5f);
}

// Stop
Pcm::SDL2::StopSoundEffect(channel);
```

#### OpenAL 3D Positional Audio

```cpp
#include <gel/music/OpenAL/p_audio.h>

// Initialize
Pcm::OpenAL::Audio_Init();

// Set listener position (camera/player position)
Pcm::OpenAL::Set3DListenerPosition(0.0f, 0.0f, 0.0f);
Pcm::OpenAL::Set3DListenerOrientation(0.0f, 0.0f, -1.0f,  // forward
                                       0.0f, 1.0f, 0.0f);  // up

// Load 3D sound
uint32 checksum = 0x12345678;
Pcm::OpenAL::LoadSoundEffect(checksum, "Data/Audio/SFX/engine.wav", true);

// Play at world position
int source = Pcm::OpenAL::PlaySoundEffect(checksum);
Pcm::OpenAL::Set3DSoundPosition(source, 10.0f, 0.0f, 20.0f);

// Set attenuation
Pcm::OpenAL::Set3DSoundAttenuation(source, 
    1.0f,   // rolloff factor
    10.0f,  // reference distance
    100.0f  // max distance
);

// Update listener each frame
Pcm::OpenAL::Set3DListenerPosition(playerX, playerY, playerZ);
Pcm::OpenAL::Audio_Update();
```

## Video System

### Supported Video Formats

With FFmpeg backend:
- ✅ MP4 (H.264, H.265)
- ✅ AVI
- ✅ MOV (QuickTime)
- ✅ WMV (Windows Media)
- ✅ WebM
- ✅ Many others (depends on FFmpeg build)

### Features

- ✅ Video decoding
- ✅ Audio track support
- ✅ Playback controls (play, pause, stop, seek)
- ✅ Frame-by-frame access
- ✅ Loop playback
- ✅ Volume control
- ✅ Render to texture
- ✅ Render to screen

### Building with Video Support

#### FFmpeg

```bash
# Linux
sudo apt install ffmpeg libavcodec-dev libavformat-dev libavutil-dev libswscale-dev libswresample-dev

# macOS
brew install ffmpeg

# Build
mkdir build && cd build
cmake -DUSE_FFMPEG_VIDEO=ON ..
make
```

### Usage Examples

#### Basic Video Playback

```cpp
#include <gel/Movies/FFmpeg/p_videoplayer.h>

// Initialize
Flx::FFmpeg::VideoPlayer_Init();

// Load video
if (Flx::FFmpeg::VideoPlayer_Load("Data/Movies/intro.mp4")) {
    // Get video info
    const Flx::FFmpeg::VideoInfo* info = Flx::FFmpeg::VideoPlayer_GetInfo();
    printf("Video: %dx%d @ %.2f fps, duration: %.2f sec\n",
           info->width, info->height, info->frameRate, info->duration);
    
    // Play with loop
    Flx::FFmpeg::VideoPlayer_Play(Flx::FFmpeg::VIDEO_FLAG_LOOP);
    
    // Update each frame (in game loop)
    while (Flx::FFmpeg::VideoPlayer_IsPlaying()) {
        float deltaTime = GetFrameDeltaTime();
        Flx::FFmpeg::VideoPlayer_Update(deltaTime);
        
        // Get current frame for rendering
        const Flx::FFmpeg::VideoFrame* frame = Flx::FFmpeg::VideoPlayer_GetCurrentFrame();
        // Render frame->data to screen/texture
    }
    
    // Stop and cleanup
    Flx::FFmpeg::VideoPlayer_Stop();
}

Flx::FFmpeg::VideoPlayer_Deinit();
```

#### Video with Controls

```cpp
// Pause
Flx::FFmpeg::VideoPlayer_Pause();

// Resume
Flx::FFmpeg::VideoPlayer_Resume();

// Seek to 30 seconds
Flx::FFmpeg::VideoPlayer_Seek(30.0f);

// Volume control
Flx::FFmpeg::VideoPlayer_SetVolume(0.7f);
Flx::FFmpeg::VideoPlayer_SetMuted(false);

// Check current time
float currentTime = Flx::FFmpeg::VideoPlayer_GetCurrentTime();
float totalTime = Flx::FFmpeg::VideoPlayer_GetDuration();
printf("Progress: %.1f / %.1f seconds\n", currentTime, totalTime);
```

## Asset Loader

### Features

- ✅ Unified interface for all asset types
- ✅ Reference counting
- ✅ Automatic format detection
- ✅ Async loading support
- ✅ Streaming mode for large files
- ✅ Resource caching
- ✅ Memory management

### Usage Examples

#### Basic Asset Loading

```cpp
#include <gel/AssetLoader/AssetLoader.h>

// Get singleton instance
Asset::CAssetLoader* loader = Asset::CAssetLoader::Instance();

// Initialize
loader->Init();

// Load audio asset
Asset::AssetData* audioAsset = loader->LoadAsset(
    "Data/Audio/Music/track01.ogg",
    Asset::ASSET_TYPE_AUDIO,
    Asset::ASSET_LOAD_CACHED
);

if (audioAsset && audioAsset->status == Asset::ASSET_STATUS_LOADED) {
    printf("Loaded audio: %zu bytes\n", audioAsset->dataSize);
}

// Load video asset
Asset::AssetData* videoAsset = loader->LoadAsset(
    "Data/Movies/cutscene.mp4",
    Asset::ASSET_TYPE_VIDEO,
    Asset::ASSET_LOAD_STREAMING
);

// Check if loaded
if (loader->IsAssetLoaded("Data/Movies/cutscene.mp4")) {
    printf("Video is ready\n");
}

// Unload when done
loader->UnloadAsset(audioAsset);
loader->UnloadAsset(videoAsset);

// Cleanup
loader->Deinit();
```

#### Audio-Specific Loading

```cpp
// Load with audio metadata
Asset::AudioAssetData* audioData = loader->LoadAudioAsset(
    "Data/Audio/SFX/explosion.wav",
    Asset::ASSET_LOAD_PRELOAD
);

if (audioData) {
    printf("Audio format: %d\n", audioData->format);
    printf("Sample rate: %d Hz\n", audioData->sampleRate);
    printf("Channels: %d\n", audioData->channels);
    printf("Duration: %.2f sec\n", audioData->duration);
}
```

#### Video-Specific Loading

```cpp
// Load with video metadata
Asset::VideoAssetData* videoData = loader->LoadVideoAsset(
    "Data/Movies/replay.mp4",
    Asset::ASSET_LOAD_STREAMING | Asset::ASSET_LOAD_CACHED
);

if (videoData) {
    printf("Video format: %d\n", videoData->format);
    printf("Resolution: %dx%d\n", videoData->width, videoData->height);
    printf("Frame rate: %.2f fps\n", videoData->frameRate);
    printf("Has audio: %s\n", videoData->hasAudio ? "Yes" : "No");
}
```

## Integration with Existing Systems

### Music System Integration

The new audio backends integrate with the existing `Pcm` namespace:

```cpp
// In Code/Gel/Music/music.cpp
#if defined(USE_SDL2_AUDIO)
    #include <gel/music/SDL2/p_audio.h>
    namespace AudioBackend = Pcm::SDL2;
#elif defined(USE_OPENAL_AUDIO)
    #include <gel/music/OpenAL/p_audio.h>
    namespace AudioBackend = Pcm::OpenAL;
#elif defined(USE_FMOD_AUDIO)
    #include <gel/music/FMOD/p_audio.h>
    namespace AudioBackend = Pcm::FMOD;
#endif

void Pcm::Init() {
    AudioBackend::Audio_Init();
}
```

### Movies System Integration

The FFmpeg video player integrates with the `Flx` namespace:

```cpp
// In Code/Gel/Movies/Movies.cpp
void Flx::PlayMovie(const char* pMovieName) {
#ifdef USE_FFMPEG_VIDEO
    FFmpeg::VideoPlayer_Load(pMovieName);
    FFmpeg::VideoPlayer_Play();
#endif
}
```

## Performance Considerations

### Audio

- **SDL2_mixer**: Low CPU usage, good for most games
- **OpenAL**: Medium CPU usage, excellent 3D audio quality
- **FMOD**: Configurable, can be optimized for specific needs

### Video

- **FFmpeg**: CPU usage depends on video codec and resolution
  - H.264: Moderate CPU usage
  - H.265: Higher CPU usage but better compression
  - Consider pre-encoding videos at target resolution

### Memory

- **Sound Effects**: Load into memory (typically small, <1MB each)
- **Music**: Stream from disk (saves memory)
- **Video**: Stream from disk (videos can be hundreds of MB)

## Troubleshooting

### Audio Not Playing

1. Check if audio backend is initialized:
   ```cpp
   Pcm::SDL2::Audio_Init(); // or OpenAL/FMOD
   ```

2. Verify audio file exists and is readable

3. Check volume levels:
   ```cpp
   Pcm::SDL2::Audio_SetVolume(1.0f);
   ```

4. Ensure correct format support for backend

### Video Not Playing

1. Check if FFmpeg is linked:
   ```bash
   ldd thug | grep av
   ```

2. Verify video file format is supported

3. Check FFmpeg logs for decoder errors

4. Ensure video codecs are available in FFmpeg build

### Build Errors

1. **SDL2 not found**: Install development packages
2. **OpenAL not found**: Install OpenAL-soft development packages
3. **FFmpeg not found**: Install FFmpeg development packages
4. **Linker errors**: Ensure library paths are correct

## Future Enhancements

### Planned Features

- [ ] MP3 decoder for OpenAL (via libmpg123)
- [ ] Hardware-accelerated video decoding (VAAPI/NVDEC)
- [ ] Audio streaming from compressed archives
- [ ] Multiple simultaneous video playback
- [ ] Custom DSP effects chain
- [ ] Audio capture/recording
- [ ] MIDI playback support

### API Extensions

- [ ] Async asset loading with callbacks
- [ ] Streaming audio from network
- [ ] Audio visualization data
- [ ] Subtitle support for videos
- [ ] Chapter/marker support

## License Notes

- **SDL2/SDL2_mixer**: zlib license (permissive)
- **OpenAL**: LGPL (can be linked dynamically)
- **FMOD**: Commercial license required for released games
- **FFmpeg**: LGPL/GPL depending on build configuration

Always verify licensing requirements for your specific use case.

## References

- [SDL2_mixer Documentation](https://www.libsdl.org/projects/SDL_mixer/)
- [OpenAL Programmer's Guide](https://www.openal.org/documentation/)
- [FMOD Documentation](https://www.fmod.com/docs)
- [FFmpeg Documentation](https://ffmpeg.org/documentation.html)

---

*Last updated: December 2024*

# DirectSound Audio Backend

This directory contains the DirectSound audio backend implementation for Tony Hawk's Underground (THUG) PC port.

## Overview

DirectSound is Microsoft's legacy audio API for Windows, originally part of DirectX. While deprecated by Microsoft in favor of newer APIs like XAudio2 and WASAPI, this implementation is provided for:

1. **Historical accuracy** - DirectSound was the audio API used in the original PC version
2. **Xbox compatibility** - The Xbox used DirectSound-based audio, so this backend provides compatibility with Xbox-specific audio code paths
3. **Reference implementation** - Serves as a template for other audio backends

## Implementation Status

**Current Status: Stub Implementation**

All functions are currently stubs that:
- Accept and validate parameters
- Maintain internal state (volume, streaming status)
- Return success/failure appropriately
- Provide comments indicating what a full implementation would do

A full implementation would require:
- DirectSound SDK (dsound.h, dsound.lib)
- Windows-specific code
- Audio file format parsing (WAV, MP3, OGG, etc.)
- Buffer management and streaming logic

## Files

- `p_audio.h` - DirectSound backend interface declarations
- `p_audio.cpp` - DirectSound backend stub implementations

## Key Features

### DirectSoundDoWork()

This function is specifically provided for compatibility with Xbox platform code that calls `DirectSoundDoWork()` during intensive operations (like park editor rebuilding). It ensures audio processing continues during CPU-intensive tasks.

```cpp
// Called from Code/Sk/ParkEditor2/EdMap.cpp
#ifdef __PLAT_XBOX__
DirectSoundDoWork();
Pcm::Update();
#endif
```

### Audio Backend Functions

All standard Pcm audio backend functions are implemented:

- `Audio_Init()` - Initialize DirectSound device
- `Audio_Deinit()` - Clean up DirectSound resources
- `Audio_Update()` - Per-frame audio processing
- `Audio_TrackExists()` - Check if music track file exists
- `Audio_LoadMusicHeader()` - Load audio file header
- `Audio_PreLoadMusicStream()` - Preload audio data
- `Audio_StartStreaming()` - Start music playback
- `Audio_StopStreaming()` - Stop music playback
- `Audio_PauseStream()` - Pause/unpause music
- `Audio_SetVolume()` / `Audio_GetVolume()` - Stream volume control
- `Audio_SetMusicVolume()` / `Audio_GetMusicVolume()` - Music volume control

## Building with DirectSound

To enable the DirectSound backend when building:

```bash
cd build
cmake -DAUDIO_BACKEND=DirectSound ..
cmake --build .
```

**Note**: DirectSound is only available on Windows. On other platforms, consider using:
- `SDL2` - Cross-platform, simple API (recommended)
- `OpenAL` - Cross-platform, 3D positional audio
- `FMOD` - Professional features (requires license)

## Future Implementation

A full DirectSound implementation would include:

1. **Device Initialization**
   ```cpp
   DirectSoundCreate8(NULL, &g_pDirectSound, NULL);
   g_pDirectSound->SetCooperativeLevel(hWnd, DSSCL_PRIORITY);
   ```

2. **Buffer Creation**
   - Primary buffer for mixing
   - Secondary buffers for music and sound effects
   - Streaming buffer management

3. **Audio Streaming**
   - Load audio file formats (WAV, MP3, OGG)
   - Stream data to DirectSound buffers
   - Handle buffer notifications for continuous playback

4. **Volume Control**
   - Convert 0.0-1.0 volume to DirectSound units (DSBVOLUME_MIN to DSBVOLUME_MAX)
   - Apply volume to secondary buffers

5. **3D Audio** (optional)
   - Set up listener and source positions
   - Use DirectSound3D for positional audio

## Alternatives to DirectSound

Microsoft deprecated DirectSound in favor of:

- **XAudio2** (DirectX 11+) - Modern replacement for DirectSound
- **WASAPI** - Low-latency Windows audio API
- **SDL2_mixer** - Cross-platform, simpler API
- **OpenAL** - Cross-platform, 3D audio support
- **FMOD** - Professional game audio middleware

For a modern PC port, SDL2_mixer or OpenAL are recommended over DirectSound.

## References

- [DirectSound Documentation (Archived)](https://docs.microsoft.com/en-us/previous-versions/windows/desktop/ee416960(v=vs.85))
- [DirectSound Programming Guide](https://docs.microsoft.com/en-us/windows/win32/xaudio2/programming-guide)
- Xbox Audio Programming Guide (for DirectSoundDoWork background)

## See Also

- `Code/Gel/Music/SDL2/` - SDL2_mixer backend (recommended)
- `Code/Gel/Music/OpenAL/` - OpenAL backend
- `Code/Gel/Music/FMOD/` - FMOD backend
- `docs/subsystems/AUDIO.md` - Audio system architecture

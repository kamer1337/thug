# SDL2_mixer Audio Backend

This directory contains the SDL2_mixer-based audio backend for THUG PC port.

## Status

**STUB IMPLEMENTATION** - Interface structure provided, requires SDL2_mixer API implementation.

## Purpose

SDL2_mixer provides:
- Multi-channel audio mixing
- Music playback (MP3, OGG, FLAC, MOD, etc.)
- Sound effect playback
- Volume control
- Cross-platform support

## Implementation Requirements

1. **SDL2** - Simple DirectMedia Layer 2.0+
2. **SDL2_mixer** - SDL audio mixer library
3. **Linking** - SDL2.lib, SDL2_mixer.lib

## Advantages

- **Cross-platform** - Windows, Linux, macOS, and more
- **Simple API** - Easy to use and integrate
- **Format support** - Multiple audio formats out of the box
- **Active development** - Well-maintained library
- **Free and open source** - MIT license

## Files

- `p_music.h` - Music system interface
- `p_music.cpp` - SDL2_mixer implementation (stub)
- `README.md` - This file

## Integration

Replace the stub implementations in `Code/Gel/Music/Win32/p_music.h` with SDL2_mixer calls.

## Example Implementation Pattern

```cpp
// Initialize SDL2_mixer
bool PCMAudio_Init() {
    if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0) {
        return false;
    }
    Mix_AllocateChannels(16);
    return true;
}

// Load and play music
bool PCMAudio_StartStreaming() {
    // Mix_PlayMusic(music, -1);
    return true;
}
```

## References

- [SDL2_mixer Documentation](https://www.libsdl.org/projects/SDL_mixer/docs/SDL_mixer.html)
- [SDL2 Documentation](https://wiki.libsdl.org/)

## See Also

- `Code/Gel/Music/OpenAL/` - OpenAL alternative backend
- `Code/Gel/Music/FMOD/` - FMOD alternative backend

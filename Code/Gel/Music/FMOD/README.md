# FMOD Audio Backend

This directory contains the FMOD-based audio backend for THUG PC port.

## Status

**STUB IMPLEMENTATION** - Interface structure provided, requires FMOD API implementation.

## Purpose

FMOD provides:
- Advanced audio mixing
- 3D positional audio
- Real-time DSP effects
- Streaming audio
- Multi-platform support
- Professional features

## Implementation Requirements

1. **FMOD Core** - FMOD Studio API or FMOD Core API
2. **Headers** - fmod.h, fmod.hpp
3. **Linking** - fmod.lib (or fmodL.lib for debug)
4. **License** - FMOD is free for non-commercial use, requires license for commercial

## Advantages

- **Professional grade** - Used in AAA games
- **Advanced features** - DSP, effects, mixing, profiling
- **3D audio** - Excellent positional audio support
- **Cross-platform** - Windows, Linux, macOS, consoles, mobile
- **Active development** - Regular updates and support

## Licensing Note

**IMPORTANT**: FMOD requires a commercial license for commercial use. For open-source or educational projects, FMOD offers free licenses. Check [FMOD Licensing](https://www.fmod.com/licensing) for details.

## Files

- `p_audio.h` - Audio system interface
- `p_audio.cpp` - FMOD implementation (stub)
- `README.md` - This file

## Integration

Implement the audio functions using FMOD API calls.

## Example Implementation Pattern

```cpp
// Initialize FMOD
bool AudioInit() {
    FMOD::System* system = NULL;
    FMOD::System_Create(&system);
    system->init(512, FMOD_INIT_NORMAL, 0);
    return true;
}

// Play sound
void PlaySound(const char* filename, float x, float y, float z) {
    FMOD::Sound* sound = NULL;
    system->createSound(filename, FMOD_3D, 0, &sound);
    FMOD::Channel* channel = NULL;
    system->playSound(sound, 0, false, &channel);
    channel->set3DAttributes(&pos, &vel);
}
```

## References

- [FMOD Documentation](https://www.fmod.com/docs/)
- [FMOD Core API](https://www.fmod.com/docs/2.02/api/core-api.html)
- [FMOD Licensing](https://www.fmod.com/licensing)

## See Also

- `Code/Gel/Music/SDL2/` - SDL2_mixer alternative backend (free, simpler)
- `Code/Gel/Music/OpenAL/` - OpenAL alternative backend (free, 3D audio)

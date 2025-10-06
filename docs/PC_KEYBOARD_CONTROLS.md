# PC Keyboard Controls for THUG

This document describes the keyboard controls implemented for the PC port of Tony Hawk's Underground.

## Overview

The PC port now supports keyboard controls as an alternative to gamepad input. The keyboard keys are mapped to simulate a PlayStation 2 controller layout.

## Control Mappings

### Movement Controls

| Key | Controller Button | Function |
|-----|------------------|----------|
| W or ↑ | D-Pad Up | Forward/Up |
| S or ↓ | D-Pad Down | Backward/Down |
| A or ← | D-Pad Left | Left |
| D or → | D-Pad Right | Right |

### Action Buttons

| Key | Controller Button | Function |
|-----|------------------|----------|
| Space | X (Cross) | Jump/Ollie |
| Shift | Square | Grind/Grab tricks |
| Ctrl | Circle | Flip tricks |
| Alt | Triangle | Special tricks/Nollie |

### Shoulder Buttons

| Key | Controller Button | Function |
|-----|------------------|----------|
| Q | L1 | Spin Left |
| E | R1 | Spin Right |
| Z | L2 | Modifier button |
| C | R2 | Modifier button |

### Camera Control (Right Analog Stick)

| Key | Function |
|-----|----------|
| I | Camera Up |
| K | Camera Down |
| J | Camera Left |
| L | Camera Right |

### System Buttons

| Key | Controller Button | Function |
|-----|------------------|----------|
| Enter | Start | Pause menu |
| Tab | Select | View menu |

## Implementation Details

### Architecture

The keyboard input system is implemented in three main components:

1. **Keyboard Reading** (`Code/Sys/SIO/Win32/p_keyboard.cpp`)
   - Reads raw keyboard input using Win32 API (`GetAsyncKeyState`)
   - Tracks key state changes (makes/breaks)
   - Provides character input for text entry

2. **Keyboard-to-Controller Mapping** (`Code/Sys/SIO/Win32/p_siodev.cpp`)
   - Converts keyboard input to PS2 controller data format
   - Maps keys to digital button states
   - Simulates analog stick input from keyboard
   - Provides pressure-sensitive button values

3. **Input Integration** (`Code/Sys/SIO/siodev.cpp`)
   - Integrates keyboard input into existing input system
   - Uses conditional compilation for Win32 platform
   - Maintains compatibility with gamepad input

### Technical Notes

- Keyboard input is processed every frame using `GetAsyncKeyState()`
- The system simulates a PlayStation 2 DualShock 2 controller
- Digital buttons use active-low logic (bit cleared = pressed)
- Analog values range from 0x00 (minimum) to 0xFF (maximum) with 0x80 as center
- Multiple keys can be pressed simultaneously for combo moves
- The keyboard is always considered "connected" (no plug/unplug detection)

### Compatibility

- **Gamepad Support**: Gamepad controls remain fully functional
- **Platform**: Win32/PC only (uses Win32 API)
- **Build**: Requires `__PLAT_WN32__` preprocessor definition

## Building

The keyboard control implementation is automatically included when building for Win32:

```bash
mkdir build
cd build
cmake ..
cmake --build .
```

The CMake configuration will detect the Win32 platform and include the keyboard implementation files.

## Customization

To modify the key mappings, edit `Code/Sys/SIO/Win32/p_siodev.cpp` in the `read_data_keyboard()` function. The mapping section clearly shows which keys correspond to which controller buttons.

Example:
```cpp
// Change jump button from Space to 'J'
bool x_button = (GetAsyncKeyState('J') & 0x8000) != 0;
```

## Future Enhancements

Potential improvements for the keyboard control system:

- [ ] Configurable key bindings through settings file
- [ ] Mouse support for camera control
- [ ] Analog movement simulation with acceleration
- [ ] Multiple keyboard layouts (AZERTY, etc.)
- [ ] On-screen keyboard indicator
- [ ] Key binding menu in-game

## Troubleshooting

### Keys Not Responding

1. Ensure the game window has focus
2. Check that the keyboard is enabled (not disabled by frontend)
3. Verify the build includes Win32 platform code

### Multiple Keys Not Working

Some keyboards have limitations on simultaneous key presses (ghosting). Try different key combinations or use a gaming keyboard with N-key rollover.

### Analog Stick Issues

The keyboard simulates digital input on analog sticks. For precise analog control, a gamepad is recommended.

## Credits

- Original Game: Neversoft Entertainment / Activision
- PC Keyboard Implementation: PC Port Development Team

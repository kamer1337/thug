# SDL2 Window Management

## Overview

Cross-platform window management implementation using SDL2. This module provides a simple, consistent API for creating and managing application windows across Linux, macOS, and Windows platforms.

## Features

- **Window Creation**: Create customizable application windows with various flags
- **Event Handling**: Unified event system for window events, keyboard input, and mouse input
- **Fullscreen Support**: Toggle between windowed and fullscreen modes
- **Window Management**: Control window visibility, position, size, and state

## Requirements

### Build-time
- CMake 3.10 or higher
- SDL2 development libraries

### Runtime
- SDL2 libraries (libSDL2)

### Installation

#### Ubuntu/Debian
```bash
sudo apt install libsdl2-dev
```

#### macOS
```bash
brew install sdl2
```

#### Windows
Download SDL2 development libraries from https://www.libsdl.org/download-2.0.php

## Usage

### Building with SDL2 Window Support

Configure the build with SDL2 window support:

```bash
cmake -DUSE_SDL2_WINDOW=ON ..
make
```

### Basic Example

```cpp
#include <sys/SDL2/Window.h>

int main()
{
    // Initialize SDL2
    if (!Sys::SDL2::Init())
    {
        return 1;
    }
    
    // Configure window
    Sys::SDL2::WindowConfig config;
    config.title = "My Game";
    config.width = 1280;
    config.height = 720;
    config.flags = Sys::SDL2::WINDOW_RESIZABLE;
    
    // Create window
    if (!Sys::SDL2::CreateWindow(config))
    {
        Sys::SDL2::Deinit();
        return 1;
    }
    
    // Main loop
    bool running = true;
    while (running)
    {
        Sys::SDL2::Event event;
        while (Sys::SDL2::PollEvent(event))
        {
            if (event.type == Sys::SDL2::EVENT_QUIT ||
                event.type == Sys::SDL2::EVENT_WINDOW_CLOSE)
            {
                running = false;
            }
        }
        
        // Render frame...
        Sys::SDL2::UpdateWindow();
    }
    
    // Cleanup
    Sys::SDL2::Deinit();
    return 0;
}
```

## API Reference

### Initialization

- `bool Init()` - Initialize SDL2 subsystems
- `void Deinit()` - Shutdown SDL2 subsystems

### Window Creation

- `bool CreateWindow(const WindowConfig& config)` - Create a window with specified configuration
- `void DestroyWindow()` - Destroy the current window

### Event Handling

- `bool PollEvent(Event& event)` - Poll for the next event (returns true if event available)

### Window Properties

- `void SetWindowTitle(const char* title)` - Set window title
- `void GetWindowSize(int& width, int& height)` - Get current window size
- `void SetWindowSize(int width, int height)` - Set window size
- `void GetWindowPosition(int& x, int& y)` - Get window position
- `void SetWindowPosition(int x, int y)` - Set window position

### Fullscreen

- `bool SetFullscreen(bool fullscreen)` - Set fullscreen mode
- `bool ToggleFullscreen()` - Toggle between windowed and fullscreen
- `bool IsFullscreen()` - Check if window is in fullscreen mode

### Window State

- `void SetWindowVisible(bool visible)` - Show or hide window
- `bool IsWindowVisible()` - Check if window is visible
- `void MinimizeWindow()` - Minimize window
- `void MaximizeWindow()` - Maximize window
- `void RestoreWindow()` - Restore window from minimized/maximized state
- `void RaiseWindow()` - Bring window to front

### Display

- `void UpdateWindow()` - Update window display (swap buffers)

## Window Flags

When creating a window, you can combine the following flags:

- `WINDOW_NONE` - No special flags (default)
- `WINDOW_FULLSCREEN` - Create window in fullscreen mode
- `WINDOW_RESIZABLE` - Allow window to be resized
- `WINDOW_BORDERLESS` - Create borderless window
- `WINDOW_HIDDEN` - Create window hidden (can be shown later)
- `WINDOW_MAXIMIZED` - Create window maximized

Example:
```cpp
config.flags = Sys::SDL2::WINDOW_RESIZABLE | Sys::SDL2::WINDOW_MAXIMIZED;
```

## Event Types

The event system provides the following event types:

- `EVENT_NONE` - No event
- `EVENT_QUIT` - Application quit requested
- `EVENT_WINDOW_CLOSE` - Window close button clicked
- `EVENT_WINDOW_RESIZE` - Window resized
- `EVENT_WINDOW_FOCUS_GAINED` - Window gained focus
- `EVENT_WINDOW_FOCUS_LOST` - Window lost focus
- `EVENT_KEY_DOWN` - Key pressed
- `EVENT_KEY_UP` - Key released
- `EVENT_MOUSE_BUTTON_DOWN` - Mouse button pressed
- `EVENT_MOUSE_BUTTON_UP` - Mouse button released
- `EVENT_MOUSE_MOTION` - Mouse moved

## Integration with Graphics APIs

The SDL2 window can be used with various graphics APIs:

- **OpenGL**: Use `SDL_GL_CreateContext()` after window creation
- **Vulkan**: Use `SDL_Vulkan_CreateSurface()` for Vulkan surface
- **Software Rendering**: Use `SDL_GetWindowSurface()` for software rendering

## Notes

- The implementation uses SDL2's desktop fullscreen mode for better compatibility
- Window dimensions are automatically saved when switching to fullscreen and restored when returning to windowed mode
- When SDL2 window support is not enabled (`USE_SDL2_WINDOW` not defined), all functions provide stub implementations that return appropriate error values

## Related Tasks

- TASK-PLATFORM-003: SDL2 Window Management (this implementation)
- See ROADMAP.md Section 9.1 for overall SDL2 port strategy

## License

Copyright (C) 1999 - Neversoft Entertainment - All Rights Reserved

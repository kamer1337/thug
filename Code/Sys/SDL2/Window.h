/*****************************************************************************
**																			**
**					   	  Neversoft Entertainment							**
**																		   	**
**				   Copyright (C) 1999 - All Rights Reserved				   	**
**																			**
******************************************************************************
**																			**
**	Project:		SYS Library												**
**																			**
**	Module:			SDL2 Window Management									**
**																			**
**	File name:		SDL2/Window.h											**
**																			**
**	Created by:		PC Port													**
**																			**
**	Description:	Cross-platform window management with SDL2				**
**																			**
*****************************************************************************/

#ifndef __SYS_SDL2_WINDOW_H
#define __SYS_SDL2_WINDOW_H

/*****************************************************************************
**							  	  Includes									**
*****************************************************************************/

// For standalone builds, we can skip core/defines.h
#ifndef __CORE_DEFINES_H
#ifndef __SDL2_WINDOW_STANDALONE__
#include <core/defines.h>
#else
// Minimal type definitions for standalone builds
#include <stdint.h>
typedef uint32_t uint32;
#endif
#endif

#ifdef USE_SDL2_WINDOW
#include <SDL2/SDL.h>
#endif

/*****************************************************************************
**								   Defines									**
*****************************************************************************/

namespace Sys
{
namespace SDL2
{

/*****************************************************************************
**							     Type Defines								**
*****************************************************************************/

// Window creation flags
enum WindowFlags
{
	WINDOW_NONE				= 0,
	WINDOW_FULLSCREEN		= (1 << 0),
	WINDOW_RESIZABLE		= (1 << 1),
	WINDOW_BORDERLESS		= (1 << 2),
	WINDOW_HIDDEN			= (1 << 3),
	WINDOW_MAXIMIZED		= (1 << 4),
};

// Event types handled by the window system
enum EventType
{
	EVENT_NONE = 0,
	EVENT_QUIT,
	EVENT_WINDOW_CLOSE,
	EVENT_WINDOW_RESIZE,
	EVENT_WINDOW_FOCUS_GAINED,
	EVENT_WINDOW_FOCUS_LOST,
	EVENT_KEY_DOWN,
	EVENT_KEY_UP,
	EVENT_MOUSE_BUTTON_DOWN,
	EVENT_MOUSE_BUTTON_UP,
	EVENT_MOUSE_MOTION,
};

// Window configuration structure
struct WindowConfig
{
	const char* title;
	int width;
	int height;
	int x;
	int y;
	uint32 flags;
	
	WindowConfig()
		: title("THUG")
		, width(1280)
		, height(720)
		, x(SDL_WINDOWPOS_CENTERED)
		, y(SDL_WINDOWPOS_CENTERED)
		, flags(WINDOW_NONE)
	{
	}
};

// Event data structure
struct Event
{
	EventType type;
	int key_code;
	int mouse_x;
	int mouse_y;
	int mouse_button;
	int window_width;
	int window_height;
	
	Event() : type(EVENT_NONE), key_code(0), mouse_x(0), mouse_y(0),
	          mouse_button(0), window_width(0), window_height(0) {}
};

/*****************************************************************************
**							   Public Prototypes							**
*****************************************************************************/

// Initialize SDL2 subsystems
bool	Init( void );

// Shutdown SDL2 subsystems
void	Deinit( void );

// Create a window
bool	CreateWindow( const WindowConfig& config );

// Destroy the window
void	DestroyWindow( void );

// Get the SDL window handle
#ifdef USE_SDL2_WINDOW
SDL_Window*	GetWindow( void );
#endif

// Poll for events
bool	PollEvent( Event& event );

// Set window title
void	SetWindowTitle( const char* title );

// Get window size
void	GetWindowSize( int& width, int& height );

// Set window size
void	SetWindowSize( int width, int height );

// Set fullscreen mode
bool	SetFullscreen( bool fullscreen );

// Toggle fullscreen mode
bool	ToggleFullscreen( void );

// Is window fullscreen
bool	IsFullscreen( void );

// Show/hide window
void	SetWindowVisible( bool visible );

// Check if window is visible
bool	IsWindowVisible( void );

// Set window position
void	SetWindowPosition( int x, int y );

// Get window position
void	GetWindowPosition( int& x, int& y );

// Minimize window
void	MinimizeWindow( void );

// Maximize window
void	MaximizeWindow( void );

// Restore window
void	RestoreWindow( void );

// Raise window
void	RaiseWindow( void );

// Update window (swap buffers if needed)
void	UpdateWindow( void );

} // namespace SDL2
} // namespace Sys

#endif	// __SYS_SDL2_WINDOW_H

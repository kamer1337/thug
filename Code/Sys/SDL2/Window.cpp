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
**	File name:		SDL2/Window.cpp											**
**																			**
**	Created by:		PC Port													**
**																			**
**	Description:	Cross-platform window management implementation			**
**																			**
*****************************************************************************/

/*****************************************************************************
**							  	  Includes									**
*****************************************************************************/

#include <sys/SDL2/Window.h>

#ifndef __SDL2_WINDOW_STANDALONE__
#include <core/defines.h>
#endif

#ifdef USE_SDL2_WINDOW
#include <SDL2/SDL.h>
#include <stdio.h>
#include <string.h>
#endif

/*****************************************************************************
**								   Defines									**
*****************************************************************************/

namespace Sys
{
namespace SDL2
{

#ifdef USE_SDL2_WINDOW

/*****************************************************************************
**								 Private Data								**
*****************************************************************************/

// Global SDL2 window state
static SDL_Window* s_window = nullptr;
static bool s_initialized = false;
static bool s_fullscreen = false;
static int s_windowed_width = 1280;
static int s_windowed_height = 720;

/*****************************************************************************
**							   Private Functions							**
*****************************************************************************/

/******************************************************************/
/*                                                                */
/*  Convert SDL2 event to our event structure                    */
/*                                                                */
/******************************************************************/

static bool ConvertSDLEvent( const SDL_Event& sdl_event, Event& event )
{
	event.type = EVENT_NONE;
	
	switch ( sdl_event.type )
	{
		case SDL_QUIT:
			event.type = EVENT_QUIT;
			return true;
			
		case SDL_WINDOWEVENT:
			switch ( sdl_event.window.event )
			{
				case SDL_WINDOWEVENT_CLOSE:
					event.type = EVENT_WINDOW_CLOSE;
					return true;
					
				case SDL_WINDOWEVENT_RESIZED:
				case SDL_WINDOWEVENT_SIZE_CHANGED:
					event.type = EVENT_WINDOW_RESIZE;
					event.window_width = sdl_event.window.data1;
					event.window_height = sdl_event.window.data2;
					return true;
					
				case SDL_WINDOWEVENT_FOCUS_GAINED:
					event.type = EVENT_WINDOW_FOCUS_GAINED;
					return true;
					
				case SDL_WINDOWEVENT_FOCUS_LOST:
					event.type = EVENT_WINDOW_FOCUS_LOST;
					return true;
					
				default:
					break;
			}
			break;
			
		case SDL_KEYDOWN:
			event.type = EVENT_KEY_DOWN;
			event.key_code = sdl_event.key.keysym.sym;
			return true;
			
		case SDL_KEYUP:
			event.type = EVENT_KEY_UP;
			event.key_code = sdl_event.key.keysym.sym;
			return true;
			
		case SDL_MOUSEBUTTONDOWN:
			event.type = EVENT_MOUSE_BUTTON_DOWN;
			event.mouse_button = sdl_event.button.button;
			event.mouse_x = sdl_event.button.x;
			event.mouse_y = sdl_event.button.y;
			return true;
			
		case SDL_MOUSEBUTTONUP:
			event.type = EVENT_MOUSE_BUTTON_UP;
			event.mouse_button = sdl_event.button.button;
			event.mouse_x = sdl_event.button.x;
			event.mouse_y = sdl_event.button.y;
			return true;
			
		case SDL_MOUSEMOTION:
			event.type = EVENT_MOUSE_MOTION;
			event.mouse_x = sdl_event.motion.x;
			event.mouse_y = sdl_event.motion.y;
			return true;
			
		default:
			break;
	}
	
	return false;
}

/*****************************************************************************
**							   Public Functions								**
*****************************************************************************/

/******************************************************************/
/*                                                                */
/*  Initialize SDL2 subsystems                                   */
/*                                                                */
/******************************************************************/

bool Init( void )
{
	if ( s_initialized )
	{
		printf( "SDL2 Window: Already initialized\n" );
		return true;
	}
	
	// Initialize SDL2 video subsystem
	if ( SDL_Init( SDL_INIT_VIDEO ) < 0 )
	{
		printf( "SDL2 Window: Failed to initialize SDL2: %s\n", SDL_GetError() );
		return false;
	}
	
	s_initialized = true;
	printf( "SDL2 Window: Initialized successfully\n" );
	
	return true;
}

/******************************************************************/
/*                                                                */
/*  Shutdown SDL2 subsystems                                     */
/*                                                                */
/******************************************************************/

void Deinit( void )
{
	if ( !s_initialized )
	{
		return;
	}
	
	DestroyWindow();
	SDL_Quit();
	
	s_initialized = false;
	printf( "SDL2 Window: Shutdown complete\n" );
}

/******************************************************************/
/*                                                                */
/*  Create a window                                              */
/*                                                                */
/******************************************************************/

bool CreateWindow( const WindowConfig& config )
{
	if ( !s_initialized )
	{
		printf( "SDL2 Window: Cannot create window - SDL2 not initialized\n" );
		return false;
	}
	
	if ( s_window )
	{
		printf( "SDL2 Window: Window already exists\n" );
		return true;
	}
	
	// Convert our flags to SDL2 flags
	Uint32 sdl_flags = SDL_WINDOW_SHOWN;
	
	if ( config.flags & WINDOW_FULLSCREEN )
	{
		sdl_flags |= SDL_WINDOW_FULLSCREEN_DESKTOP;
		s_fullscreen = true;
	}
	
	if ( config.flags & WINDOW_RESIZABLE )
	{
		sdl_flags |= SDL_WINDOW_RESIZABLE;
	}
	
	if ( config.flags & WINDOW_BORDERLESS )
	{
		sdl_flags |= SDL_WINDOW_BORDERLESS;
	}
	
	if ( config.flags & WINDOW_HIDDEN )
	{
		sdl_flags &= ~SDL_WINDOW_SHOWN;
		sdl_flags |= SDL_WINDOW_HIDDEN;
	}
	
	if ( config.flags & WINDOW_MAXIMIZED )
	{
		sdl_flags |= SDL_WINDOW_MAXIMIZED;
	}
	
	// Create the window
	s_window = SDL_CreateWindow(
		config.title,
		config.x,
		config.y,
		config.width,
		config.height,
		sdl_flags
	);
	
	if ( !s_window )
	{
		printf( "SDL2 Window: Failed to create window: %s\n", SDL_GetError() );
		return false;
	}
	
	// Store windowed dimensions
	if ( !s_fullscreen )
	{
		s_windowed_width = config.width;
		s_windowed_height = config.height;
	}
	
	printf( "SDL2 Window: Created window '%s' (%dx%d)\n", 
	        config.title, config.width, config.height );
	
	return true;
}

/******************************************************************/
/*                                                                */
/*  Destroy the window                                           */
/*                                                                */
/******************************************************************/

void DestroyWindow( void )
{
	if ( s_window )
	{
		SDL_DestroyWindow( s_window );
		s_window = nullptr;
		printf( "SDL2 Window: Window destroyed\n" );
	}
}

/******************************************************************/
/*                                                                */
/*  Get the SDL window handle                                    */
/*                                                                */
/******************************************************************/

SDL_Window* GetWindow( void )
{
	return s_window;
}

/******************************************************************/
/*                                                                */
/*  Poll for events                                              */
/*                                                                */
/******************************************************************/

bool PollEvent( Event& event )
{
	if ( !s_initialized )
	{
		return false;
	}
	
	SDL_Event sdl_event;
	
	while ( SDL_PollEvent( &sdl_event ) )
	{
		if ( ConvertSDLEvent( sdl_event, event ) )
		{
			return true;
		}
	}
	
	return false;
}

/******************************************************************/
/*                                                                */
/*  Set window title                                             */
/*                                                                */
/******************************************************************/

void SetWindowTitle( const char* title )
{
	if ( s_window && title )
	{
		SDL_SetWindowTitle( s_window, title );
	}
}

/******************************************************************/
/*                                                                */
/*  Get window size                                              */
/*                                                                */
/******************************************************************/

void GetWindowSize( int& width, int& height )
{
	if ( s_window )
	{
		SDL_GetWindowSize( s_window, &width, &height );
	}
	else
	{
		width = 0;
		height = 0;
	}
}

/******************************************************************/
/*                                                                */
/*  Set window size                                              */
/*                                                                */
/******************************************************************/

void SetWindowSize( int width, int height )
{
	if ( s_window )
	{
		SDL_SetWindowSize( s_window, width, height );
		
		if ( !s_fullscreen )
		{
			s_windowed_width = width;
			s_windowed_height = height;
		}
	}
}

/******************************************************************/
/*                                                                */
/*  Set fullscreen mode                                          */
/*                                                                */
/******************************************************************/

bool SetFullscreen( bool fullscreen )
{
	if ( !s_window )
	{
		return false;
	}
	
	Uint32 flags = fullscreen ? SDL_WINDOW_FULLSCREEN_DESKTOP : 0;
	
	if ( SDL_SetWindowFullscreen( s_window, flags ) < 0 )
	{
		printf( "SDL2 Window: Failed to set fullscreen mode: %s\n", SDL_GetError() );
		return false;
	}
	
	s_fullscreen = fullscreen;
	
	// Restore windowed size when exiting fullscreen
	if ( !fullscreen )
	{
		SDL_SetWindowSize( s_window, s_windowed_width, s_windowed_height );
		SDL_SetWindowPosition( s_window, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED );
	}
	
	printf( "SDL2 Window: Fullscreen mode %s\n", fullscreen ? "enabled" : "disabled" );
	
	return true;
}

/******************************************************************/
/*                                                                */
/*  Toggle fullscreen mode                                       */
/*                                                                */
/******************************************************************/

bool ToggleFullscreen( void )
{
	return SetFullscreen( !s_fullscreen );
}

/******************************************************************/
/*                                                                */
/*  Is window fullscreen                                         */
/*                                                                */
/******************************************************************/

bool IsFullscreen( void )
{
	return s_fullscreen;
}

/******************************************************************/
/*                                                                */
/*  Show/hide window                                             */
/*                                                                */
/******************************************************************/

void SetWindowVisible( bool visible )
{
	if ( s_window )
	{
		if ( visible )
		{
			SDL_ShowWindow( s_window );
		}
		else
		{
			SDL_HideWindow( s_window );
		}
	}
}

/******************************************************************/
/*                                                                */
/*  Check if window is visible                                   */
/*                                                                */
/******************************************************************/

bool IsWindowVisible( void )
{
	if ( s_window )
	{
		Uint32 flags = SDL_GetWindowFlags( s_window );
		return ( flags & SDL_WINDOW_SHOWN ) != 0;
	}
	
	return false;
}

/******************************************************************/
/*                                                                */
/*  Set window position                                          */
/*                                                                */
/******************************************************************/

void SetWindowPosition( int x, int y )
{
	if ( s_window )
	{
		SDL_SetWindowPosition( s_window, x, y );
	}
}

/******************************************************************/
/*                                                                */
/*  Get window position                                          */
/*                                                                */
/******************************************************************/

void GetWindowPosition( int& x, int& y )
{
	if ( s_window )
	{
		SDL_GetWindowPosition( s_window, &x, &y );
	}
	else
	{
		x = 0;
		y = 0;
	}
}

/******************************************************************/
/*                                                                */
/*  Minimize window                                              */
/*                                                                */
/******************************************************************/

void MinimizeWindow( void )
{
	if ( s_window )
	{
		SDL_MinimizeWindow( s_window );
	}
}

/******************************************************************/
/*                                                                */
/*  Maximize window                                              */
/*                                                                */
/******************************************************************/

void MaximizeWindow( void )
{
	if ( s_window )
	{
		SDL_MaximizeWindow( s_window );
	}
}

/******************************************************************/
/*                                                                */
/*  Restore window                                               */
/*                                                                */
/******************************************************************/

void RestoreWindow( void )
{
	if ( s_window )
	{
		SDL_RestoreWindow( s_window );
	}
}

/******************************************************************/
/*                                                                */
/*  Raise window                                                 */
/*                                                                */
/******************************************************************/

void RaiseWindow( void )
{
	if ( s_window )
	{
		SDL_RaiseWindow( s_window );
	}
}

/******************************************************************/
/*                                                                */
/*  Update window (swap buffers if needed)                       */
/*                                                                */
/******************************************************************/

void UpdateWindow( void )
{
	if ( s_window )
	{
		// For now, just update the window surface
		// In a real implementation, this would swap OpenGL/Vulkan buffers
		SDL_UpdateWindowSurface( s_window );
	}
}

#else // !USE_SDL2_WINDOW

/*****************************************************************************
**							   Stub Functions								**
*****************************************************************************/

// Stub implementations when SDL2 is not enabled
bool Init( void ) { return false; }
void Deinit( void ) {}
bool CreateWindow( const WindowConfig& ) { return false; }
void DestroyWindow( void ) {}
bool PollEvent( Event& ) { return false; }
void SetWindowTitle( const char* ) {}
void GetWindowSize( int&, int& ) {}
void SetWindowSize( int, int ) {}
bool SetFullscreen( bool ) { return false; }
bool ToggleFullscreen( void ) { return false; }
bool IsFullscreen( void ) { return false; }
void SetWindowVisible( bool ) {}
bool IsWindowVisible( void ) { return false; }
void SetWindowPosition( int, int ) {}
void GetWindowPosition( int&, int& ) {}
void MinimizeWindow( void ) {}
void MaximizeWindow( void ) {}
void RestoreWindow( void ) {}
void RaiseWindow( void ) {}
void UpdateWindow( void ) {}

#endif // USE_SDL2_WINDOW

} // namespace SDL2
} // namespace Sys

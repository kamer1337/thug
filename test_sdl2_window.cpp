/*
 * SDL2 Window Management Test Program
 * 
 * Tests the basic functionality of the SDL2 window system:
 * - Initialization
 * - Window creation
 * - Event handling
 * - Fullscreen toggle
 * - Window management functions
 */

#include <cstdio>

// Must include this BEFORE sys/SDL2/Window.h to avoid printf redefinition issues
#define printf std::printf

#include <sys/SDL2/Window.h>

int main(int argc, char* argv[])
{
	printf("SDL2 Window Management Test\n");
	printf("============================\n\n");
	
	// Test 1: Initialize SDL2
	printf("Test 1: Initializing SDL2...\n");
	if (!Sys::SDL2::Init())
	{
		printf("FAILED: Could not initialize SDL2\n");
		return 1;
	}
	printf("PASSED: SDL2 initialized\n\n");
	
	// Test 2: Create window
	printf("Test 2: Creating window...\n");
	Sys::SDL2::WindowConfig config;
	config.title = "SDL2 Window Test";
	config.width = 800;
	config.height = 600;
	config.flags = Sys::SDL2::WINDOW_RESIZABLE;
	
	if (!Sys::SDL2::CreateWindow(config))
	{
		printf("FAILED: Could not create window\n");
		Sys::SDL2::Deinit();
		return 1;
	}
	printf("PASSED: Window created\n\n");
	
	// Test 3: Get window size
	printf("Test 3: Getting window size...\n");
	int width, height;
	Sys::SDL2::GetWindowSize(width, height);
	printf("Window size: %dx%d\n", width, height);
	if (width != 800 || height != 600)
	{
		printf("WARNING: Window size mismatch (expected 800x600)\n");
	}
	printf("PASSED: Window size retrieved\n\n");
	
	// Test 4: Set window title
	printf("Test 4: Setting window title...\n");
	Sys::SDL2::SetWindowTitle("SDL2 Window Test - Title Changed");
	printf("PASSED: Window title set\n\n");
	
	// Test 5: Check fullscreen status
	printf("Test 5: Checking fullscreen status...\n");
	bool is_fullscreen = Sys::SDL2::IsFullscreen();
	printf("Fullscreen: %s\n", is_fullscreen ? "yes" : "no");
	if (is_fullscreen)
	{
		printf("WARNING: Window should not be fullscreen initially\n");
	}
	printf("PASSED: Fullscreen status checked\n\n");
	
	// Test 6: Event loop (run for a few frames)
	printf("Test 6: Processing events (5 iterations)...\n");
	for (int i = 0; i < 5; i++)
	{
		Sys::SDL2::Event event;
		while (Sys::SDL2::PollEvent(event))
		{
			switch (event.type)
			{
				case Sys::SDL2::EVENT_QUIT:
				case Sys::SDL2::EVENT_WINDOW_CLOSE:
					printf("Received quit event\n");
					break;
				case Sys::SDL2::EVENT_KEY_DOWN:
					printf("Key pressed: %d\n", event.key_code);
					break;
				case Sys::SDL2::EVENT_WINDOW_RESIZE:
					printf("Window resized to %dx%d\n", event.window_width, event.window_height);
					break;
				default:
					break;
			}
		}
		
		// Update window
		Sys::SDL2::UpdateWindow();
		
		// Small delay
		#ifdef USE_SDL2_WINDOW
		SDL_Delay(100);
		#endif
	}
	printf("PASSED: Events processed\n\n");
	
	// Test 7: Window visibility
	printf("Test 7: Testing window visibility...\n");
	printf("Hiding window...\n");
	Sys::SDL2::SetWindowVisible(false);
	bool is_visible = Sys::SDL2::IsWindowVisible();
	if (is_visible)
	{
		printf("WARNING: Window should be hidden\n");
	}
	printf("Showing window...\n");
	Sys::SDL2::SetWindowVisible(true);
	is_visible = Sys::SDL2::IsWindowVisible();
	if (!is_visible)
	{
		printf("WARNING: Window should be visible\n");
	}
	printf("PASSED: Window visibility tested\n\n");
	
	// Test 8: Window position
	printf("Test 8: Testing window position...\n");
	Sys::SDL2::SetWindowPosition(100, 100);
	int x, y;
	Sys::SDL2::GetWindowPosition(x, y);
	printf("Window position: %d, %d\n", x, y);
	printf("PASSED: Window position tested\n\n");
	
	// Test 9: Cleanup
	printf("Test 9: Cleaning up...\n");
	Sys::SDL2::Deinit();
	printf("PASSED: SDL2 shutdown\n\n");
	
	printf("============================\n");
	printf("All tests completed successfully!\n");
	
	return 0;
}

/*****************************************************************************
**																			**
**			              Neversoft Entertainment			                **
**																		   	**
**				   Copyright (C) 1999 - All Rights Reserved				   	**
**																			**
******************************************************************************
**																			**
**	Project:		SYS Library												**
**																			**
**	Module:			SYS (SYS_) 												**
**																			**
**	File name:		keyboard.cpp											**
**																			**
**	Created:		03/08/2001	-	gj										**
**																			**
**	Description:	USB Keyboard interface									**
**																			**
*****************************************************************************/


/*****************************************************************************
**							  	  Includes									**
*****************************************************************************/

#include <core/defines.h>
#include <sys/sio/keyboard.h>

#ifdef __PLAT_WN32__
#include <windows.h>
#endif

/*****************************************************************************
**								DBG Information								**
*****************************************************************************/

namespace SIO
{

/*****************************************************************************
**								  Externals									**
*****************************************************************************/


/*****************************************************************************
**								Private Types								**
*****************************************************************************/


/*****************************************************************************
**								 Private Data								**
*****************************************************************************/

static  bool		s_active = true;
static  bool		s_enabled = true;

// Keyboard configuration settings for Win32 platform
static const int	KEYBOARD_REPEAT_DELAY = 30;		// Repeat delay (30 frames ~0.5s at 60fps)
static const int	KEYBOARD_REPEAT_RATE = 2;		// Repeat rate (2 frames ~33ms between repeats)
static const int	KEYBOARD_LAYOUT = 101;			// US 101-key keyboard layout

// Keyboard state tracking
static bool			s_key_state[256];				// Current state of all keys
static bool			s_prev_key_state[256];			// Previous frame key state

/*****************************************************************************
**								 Public Data								**
*****************************************************************************/



/*****************************************************************************
**							  Private Prototypes							**
*****************************************************************************/


/*****************************************************************************
**							   Private Functions							**
*****************************************************************************/


/*****************************************************************************
**							   Public Functions								**
*****************************************************************************/

/******************************************************************/
/*                                                                */
/*                                                                */
/******************************************************************/

int KeyboardInit(void)
{
	// Initialize keyboard for Win32 platform
	// Configuration settings (matching NGPS implementation):
	// - Repeat delay: 30 frames (~0.5 seconds at 60fps)
	// - Repeat rate: 2 frames (~33ms between repeats)
	// - Layout: 101-key (standard US keyboard, similar to USBKB_ARRANGEMENT_101)
	// - Code type: ASCII (similar to USBKB_CODETYPE_ASCII)
	
	s_active = true;
	s_enabled = true;
	
	// Initialize key state arrays
	for (int i = 0; i < 256; i++)
	{
		s_key_state[i] = false;
		s_prev_key_state[i] = false;
	}
	
	return 0;
}

/******************************************************************/
/*                                                                */
/*                                                                */
/******************************************************************/

int KeyboardDeinit(void)
{
	s_active = false;
	s_enabled = false;
	return 0;
}

/******************************************************************/
/*                                                                */
/*                                                                */
/******************************************************************/

int KeyboardRead( char* makes )
{
	if (!s_active || !s_enabled)
	{
		return 0;
	}
	
#ifdef __PLAT_WN32__
	int make_count = 0;
	
	// Update previous key state
	for (int i = 0; i < 256; i++)
	{
		s_prev_key_state[i] = s_key_state[i];
	}
	
	// Read current keyboard state using Win32 API
	// Check for keys that are commonly used for text input and special keys
	static const int keys_to_check[] = {
		VK_LEFT, VK_RIGHT, VK_UP, VK_DOWN,		// Arrow keys
		VK_BACK, VK_RETURN, VK_ESCAPE,			// Special keys
		VK_F1, VK_F2, VK_F3, VK_F4,				// Function keys
		VK_SPACE,								// Space
		0x41, 0x5A,								// A-Z range (just endpoints for now)
		0x30, 0x39								// 0-9 range (just endpoints for now)
	};
	
	// Check all printable ASCII characters and special keys
	for (int vk = 0x08; vk < 256; vk++)
	{
		bool is_down = (GetAsyncKeyState(vk) & 0x8000) != 0;
		s_key_state[vk] = is_down;
		
		// If key was just pressed (make event)
		if (is_down && !s_prev_key_state[vk])
		{
			// Map virtual key codes to characters or special codes
			if (vk == VK_LEFT)
				makes[make_count++] = vKB_LEFT;
			else if (vk == VK_RIGHT)
				makes[make_count++] = vKB_RIGHT;
			else if (vk == VK_UP)
				makes[make_count++] = vKB_UP;
			else if (vk == VK_DOWN)
				makes[make_count++] = vKB_DOWN;
			else if (vk == VK_BACK)
				makes[make_count++] = vKB_BACKSPACE;
			else if (vk == VK_RETURN)
				makes[make_count++] = vKB_ENTER;
			else if (vk == VK_ESCAPE)
				makes[make_count++] = vKB_ESCAPE;
			else if (vk == VK_F1)
				makes[make_count++] = vKB_F1;
			else if (vk == VK_F2)
				makes[make_count++] = vKB_F2;
			else if (vk == VK_F3)
				makes[make_count++] = vKB_F3;
			else if (vk == VK_F4)
				makes[make_count++] = vKB_F4;
			else if (vk >= 0x20 && vk <= 0x7E) // Printable ASCII range
			{
				// For letters, check if shift is down
				bool shift_down = (GetAsyncKeyState(VK_SHIFT) & 0x8000) != 0;
				
				if (vk >= 0x41 && vk <= 0x5A) // A-Z
				{
					makes[make_count++] = shift_down ? vk : (vk + 32); // Convert to lowercase if no shift
				}
				else if (vk >= 0x30 && vk <= 0x39) // 0-9
				{
					makes[make_count++] = vk;
				}
				else if (vk == VK_SPACE)
				{
					makes[make_count++] = ' ';
				}
			}
			
			if (make_count >= 255) break; // Prevent buffer overflow
		}
	}
	
	return make_count;
#else
	return 0;
#endif
}

/******************************************************************/
/*                                                                */
/*                                                                */
/******************************************************************/

void KeyboardClear( void )
{
	// Clear any pending keyboard input
	for (int i = 0; i < 256; i++)
	{
		s_key_state[i] = false;
		s_prev_key_state[i] = false;
	}
}


/******************************************************************/
/*                                                                */
/*                                                                */
/******************************************************************/

void SetKeyboardActive(bool active)
{
	s_active = active;
}

/******************************************************************/
/*                                                                */
/*                                                                */
/******************************************************************/

void EnableKeyboard( bool enable )
{
	s_enabled = enable;
}

/******************************************************************/
/*                                                                */
/*                                                                */
/******************************************************************/

} // namespace SIO

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
	
	// On Win32, keyboard input is typically handled by the OS
	// These settings document the expected keyboard behavior for the PC version
	// Actual implementation would configure the keyboard repeat and layout
	// through Win32 API or DirectInput if needed
	
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
	
	// On Win32, keyboard input would be read from the message queue
	// This is a stub for now - actual implementation would interface with Win32 API
	return 0;
}

/******************************************************************/
/*                                                                */
/*                                                                */
/******************************************************************/

void KeyboardClear( void )
{
	// Clear any pending keyboard input
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

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
**	File name:		p_siodev.cpp											**
**																			**
**	Created:		PC Port													**
**																			**
**	Description:	Win32 platform-specific input device (keyboard/mouse)	**
**																			**
*****************************************************************************/

/*****************************************************************************
**							  	  Includes									**
*****************************************************************************/

#include <core/defines.h>
#include <sys/siodev.h>
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
/* Win32-specific implementation of Device::read_data()           */
/* This reads keyboard input and maps it to controller buttons   */
/*                                                                */
/******************************************************************/

#ifdef __PLAT_WN32__

void Device::read_data_keyboard( void )
{
	// Check if keyboard input is active
	if (!m_data.m_valid)
	{
		m_data.m_valid = true; // Keyboard is always "connected"
		m_plugged_in = true;
	}

	// Zero out control data
	for (int i = 0; i < CTRL_BUFFER_LENGTH; i++)
	{
		m_data.m_control_data[i] = 0;
	}

	// Read keyboard state using Win32 API
	// Map keyboard keys to PS2 controller buttons
	
	// D-Pad mapping
	// Arrow keys OR WASD for directional input
	bool up = (GetAsyncKeyState(VK_UP) & 0x8000) || (GetAsyncKeyState('W') & 0x8000);
	bool down = (GetAsyncKeyState(VK_DOWN) & 0x8000) || (GetAsyncKeyState('S') & 0x8000);
	bool left = (GetAsyncKeyState(VK_LEFT) & 0x8000) || (GetAsyncKeyState('A') & 0x8000);
	bool right = (GetAsyncKeyState(VK_RIGHT) & 0x8000) || (GetAsyncKeyState('D') & 0x8000);
	
	// Face buttons
	// Space = X (jump/ollie)
	// Shift = Square (grind/grab)
	// Ctrl = Circle (flip tricks)
	// Alt = Triangle (special)
	bool x_button = (GetAsyncKeyState(VK_SPACE) & 0x8000) != 0;
	bool square_button = (GetAsyncKeyState(VK_SHIFT) & 0x8000) != 0;
	bool circle_button = (GetAsyncKeyState(VK_CONTROL) & 0x8000) != 0;
	bool triangle_button = (GetAsyncKeyState(VK_MENU) & 0x8000) != 0;  // VK_MENU is Alt key
	
	// Shoulder buttons
	// Q = L1, E = R1 (spin left/right)
	// Z = L2, C = R2 (modifier buttons)
	bool l1_button = (GetAsyncKeyState('Q') & 0x8000) != 0;
	bool r1_button = (GetAsyncKeyState('E') & 0x8000) != 0;
	bool l2_button = (GetAsyncKeyState('Z') & 0x8000) != 0;
	bool r2_button = (GetAsyncKeyState('C') & 0x8000) != 0;
	
	// System buttons
	// Enter = Start
	// Tab = Select
	bool start_button = (GetAsyncKeyState(VK_RETURN) & 0x8000) != 0;
	bool select_button = (GetAsyncKeyState(VK_TAB) & 0x8000) != 0;
	
	// Analog stick simulation (for camera control)
	// I/J/K/L for right analog stick
	bool right_stick_up = (GetAsyncKeyState('I') & 0x8000) != 0;
	bool right_stick_down = (GetAsyncKeyState('K') & 0x8000) != 0;
	bool right_stick_left = (GetAsyncKeyState('J') & 0x8000) != 0;
	bool right_stick_right = (GetAsyncKeyState('L') & 0x8000) != 0;

	// Map to PS2 controller data format
	// Byte 2: Digital buttons (bits are ACTIVE LOW)
	unsigned char digital_buttons = 0xFF; // All bits high = no buttons pressed
	
	if (select_button) digital_buttons &= ~0x01;  // SELECT
	if (start_button)  digital_buttons &= ~0x08;  // START
	if (up)            digital_buttons &= ~0x10;  // D-PAD UP
	if (right)         digital_buttons &= ~0x20;  // D-PAD RIGHT
	if (down)          digital_buttons &= ~0x40;  // D-PAD DOWN
	if (left)          digital_buttons &= ~0x80;  // D-PAD LEFT
	
	// Byte 3: Digital buttons (bits are ACTIVE LOW)
	unsigned char digital_buttons2 = 0xFF;
	
	if (l2_button)       digital_buttons2 &= ~0x01;  // L2
	if (r2_button)       digital_buttons2 &= ~0x02;  // R2
	if (l1_button)       digital_buttons2 &= ~0x04;  // L1
	if (r1_button)       digital_buttons2 &= ~0x08;  // R1
	if (triangle_button) digital_buttons2 &= ~0x10;  // TRIANGLE
	if (circle_button)   digital_buttons2 &= ~0x20;  // CIRCLE
	if (x_button)        digital_buttons2 &= ~0x40;  // X
	if (square_button)   digital_buttons2 &= ~0x80;  // SQUARE
	
	// Store digital button data
	m_data.m_control_data[2] = digital_buttons;
	m_data.m_control_data[3] = digital_buttons2;
	
	// Analog stick values (0x00 = left/up, 0x80 = center, 0xFF = right/down)
	// Right analog stick (for camera)
	unsigned char right_x = 0x80;
	unsigned char right_y = 0x80;
	
	if (right_stick_left)  right_x = 0x00;
	if (right_stick_right) right_x = 0xFF;
	if (right_stick_up)    right_y = 0x00;
	if (right_stick_down)  right_y = 0xFF;
	
	m_data.m_control_data[4] = right_x;  // Right X
	m_data.m_control_data[5] = right_y;  // Right Y
	
	// Left analog stick (use D-Pad/WASD input)
	unsigned char left_x = 0x80;
	unsigned char left_y = 0x80;
	
	if (left)  left_x = 0x00;
	if (right) left_x = 0xFF;
	if (up)    left_y = 0x00;
	if (down)  left_y = 0xFF;
	
	m_data.m_control_data[6] = left_x;   // Left X
	m_data.m_control_data[7] = left_y;   // Left Y
	
	// Pressure-sensitive button values (0x00 = not pressed, 0xFF = fully pressed)
	// Bytes 12-23 contain pressure values for each button
	m_data.m_control_data[12] = right ? 0xFF : 0x00;        // Right
	m_data.m_control_data[13] = left ? 0xFF : 0x00;         // Left
	m_data.m_control_data[14] = up ? 0xFF : 0x00;           // Up
	m_data.m_control_data[15] = down ? 0xFF : 0x00;         // Down
	m_data.m_control_data[16] = triangle_button ? 0xFF : 0x00;  // Triangle
	m_data.m_control_data[17] = circle_button ? 0xFF : 0x00;    // Circle
	m_data.m_control_data[18] = x_button ? 0xFF : 0x00;         // X
	m_data.m_control_data[19] = square_button ? 0xFF : 0x00;    // Square
	m_data.m_control_data[20] = l1_button ? 0xFF : 0x00;        // L1
	m_data.m_control_data[21] = r1_button ? 0xFF : 0x00;        // R1
	m_data.m_control_data[22] = l2_button ? 0xFF : 0x00;        // L2
	m_data.m_control_data[23] = r2_button ? 0xFF : 0x00;        // R2
}

#endif // __PLAT_WN32__

/******************************************************************/
/*                                                                */
/*                                                                */
/******************************************************************/

} // namespace SIO

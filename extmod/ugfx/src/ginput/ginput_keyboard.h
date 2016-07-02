/*
 * This file is subject to the terms of the GFX License. If a copy of
 * the license was not distributed with this file, you can obtain one at:
 *
 *              http://ugfx.org/license.html
 */

/**
 * @file    src/ginput/ginput_keyboard.h
 *
 * @defgroup Keyboard Keyboard
 * @ingroup GINPUT
 *
 * @brief   Sub-Module to handle physical keyboards.
 *
 * @{
 */

#ifndef _GINPUT_KEYBOARD_H
#define _GINPUT_KEYBOARD_H

#if GINPUT_NEED_KEYBOARD || GWIN_NEED_KEYBOARD || defined(__DOXYGEN__)

/*===========================================================================*/
/* Type definitions                                                          */
/*===========================================================================*/

// Event types for various ginput sources
#define GEVENT_KEYBOARD		(GEVENT_GINPUT_FIRST+2)

typedef struct GEventKeyboard_t {
	GEventType		type;				// The type of this event (GEVENT_KEYBOARD)
	uint16_t		bytecount;			// The number of bytes in c[]. Note this will only ever represent 0 or 1 characters. This is set to 0 for state transitions.
	char			c[8];				// The utf8 code for the key or a special key code
		// Normal characters with special meaning. They are a maximum of 1 byte in length.
		#define GKEY_NULL			0
		#define GKEY_BACKSPACE		8
		#define GKEY_TAB			9
		#define GKEY_LF				10
		#define GKEY_CR				13
		#define GKEY_ENTER			13
		#define GKEY_ESC			27
		#define GKEY_SPACE			32
		#define GKEY_DEL			127

		// These are special keys - GKEYSTATE_SPECIAL will be set. They are a maximum of 1 byte in length.
		#define GKEY_UP				0x81
		#define GKEY_DOWN			0x82
		#define GKEY_LEFT			0x83
		#define GKEY_RIGHT			0x84
		#define GKEY_HOME			0x85
		#define GKEY_END			0x86
		#define GKEY_PAGEUP			0x87
		#define GKEY_PAGEDOWN		0x88
		#define GKEY_INSERT			0x89
		#define GKEY_WINKEY			0x8A
		#define GKEY_RIGHTCLICKKEY	0x8B
		#define GKEY_FN1			0x91
		#define GKEY_FN2			0x92
		#define GKEY_FN3			0x93
		#define GKEY_FN4			0x94
		#define GKEY_FN5			0x95
		#define GKEY_FN6			0x96
		#define GKEY_FN7			0x97
		#define GKEY_FN8			0x98
		#define GKEY_FN9			0x99
		#define GKEY_FN10			0x9A
		#define GKEY_FN11			0x9B
		#define GKEY_FN12			0x9C
		#define GKEY_FN13			0x9D
		#define GKEY_FN14			0x9E
		#define GKEY_FN15			0x9F
		#define GKEY_CTRLBREAK		0xA0
		#define GKEY_CTRLPAUSE		0xA1
		#define GKEY_SYSREQ			0xA2
		#define GKEY_PRINTSCREEN	0xA3
		#define GKEY_POWER			0xA4
		#define GKEY_SLEEP			0xA5
		#define GKEY_SCREENSWITCH	0xA6
		#define GKEY_SCREENLOCK		0xA7
		#define GKEY_WIFIONOFF		0xA8
		#define GKEY_TRACKPADONOFF	0xA9
		#define GKEY_STARTMEDIA		0xAA
		#define GKEY_STARTHOME		0xAB
		#define GKEY_STARTEMAIL		0xAC
		#define GKEY_STARTCOMPUTER	0xAD
		#define GKEY_STARTAPP1		0xAE
		#define GKEY_STARTAPP2		0xAF
		#define GKEY_VOLUP			0xB0
		#define GKEY_VOLDOWN		0xB1
		#define GKEY_VOLMUTE		0xB2
		#define GKEY_EJECT			0xB3
		#define GKEY_MEDIAPLAY		0xB4
		#define GKEY_MEDIASTOP		0xB5
		#define GKEY_MEDIAPAUSE		0xB6
		#define GKEY_MEDIAFORWARD	0xB7
		#define GKEY_MEDIANEXT		0xB8
		#define GKEY_MEDIAREWIND	0xB9
		#define GKEY_MEDIAPREV		0xBA
		#define GKEY_MEDIASLOW		0xBB
		#define GKEY_LIGHTUP		0xBC
		#define GKEY_LIGHTDOWN		0xBD
		#define GKEY_LIGHTONOFF		0xBE
		#define GKEY_LAYOUT_FIRST	0xC0		// Special characters the layout can return start here.
		#define GKEY_DRIVER_FIRST	0xE0		// Special characters the driver can return start here.

	uint32_t		keystate;			// The keyboard state.
		#define GKEYSTATE_KEYUP_BIT			0
		#define GKEYSTATE_REPEAT_BIT		1
		#define GKEYSTATE_SPECIAL_BIT		2
		#define GKEYSTATE_RAW_BIT			3
		#define GKEYSTATE_SHIFT_L_BIT		4
		#define GKEYSTATE_SHIFT_R_BIT		5
		#define GKEYSTATE_CTRL_L_BIT		6
		#define GKEYSTATE_CTRL_R_BIT		7
		#define GKEYSTATE_ALT_L_BIT			8
		#define GKEYSTATE_ALT_R_BIT			9
		#define GKEYSTATE_FN_BIT			10
		#define GKEYSTATE_COMPOSE_BIT		11
		#define GKEYSTATE_WINKEY_BIT		12
		#define GKEYSTATE_CAPSLOCK_BIT		13
		#define GKEYSTATE_NUMLOCK_BIT		14
		#define GKEYSTATE_SCROLLLOCK_BIT	15
		#define GKEYSTATE_LAYOUT_FIRST_BIT	16
		#define GKEYSTATE_SYSTEM_FIRST_BIT	20
		#define GKEYSTATE_DRIVER_FIRST_BIT	24
		#define GKEYSTATE_MISSED_EVENT_BIT	31

		#define GKEYSTATE_KEYUP				(1<<GKEYSTATE_KEYUP_BIT)		// This is a keyup rather than a keydown event
		#define GKEYSTATE_REPEAT			(1<<GKEYSTATE_REPEAT_BIT)		// This is an automatic repeat character
		#define GKEYSTATE_SPECIAL			(1<<GKEYSTATE_SPECIAL_BIT)		// The character is a special character defined above and not a normal (utf8) character
		#define GKEYSTATE_RAW				(1<<GKEYSTATE_RAW_BIT)			// The character is a raw scancode and not a normal character
		#define GKEYSTATE_SHIFT_L			(1<<GKEYSTATE_SHIFT_L_BIT)		// Left Shift is down
		#define GKEYSTATE_SHIFT_R			(1<<GKEYSTATE_SHIFT_R_BIT)		// Right Shift is down
		#define GKEYSTATE_CTRL_L			(1<<GKEYSTATE_CTRL_L_BIT)		// Left Control is down
		#define GKEYSTATE_CTRL_R			(1<<GKEYSTATE_CTRL_R_BIT)		// Right Control is down
		#define GKEYSTATE_APPLE_CMD_L		(1<<GKEYSTATE_CTRL_L_BIT)		// Left Cmd (apple keyboard) is down
		#define GKEYSTATE_APPLE_CMD_R		(1<<GKEYSTATE_CTRL_R_BIT)		// Right Cmd (apple keyboard) is down
		#define GKEYSTATE_ALT_L				(1<<GKEYSTATE_ALT_L_BIT)		// Left Alt is down
		#define GKEYSTATE_ALT_R				(1<<GKEYSTATE_ALT_R_BIT)		// Right Alt is down
		#define GKEYSTATE_APPLE_OPTION_L	(1<<GKEYSTATE_ALT_L_BIT)		// Left Option (apple keyboard) is down
		#define GKEYSTATE_APPLE_OPTION_R	(1<<GKEYSTATE_ALT_R_BIT)		// Right Option (apple keyboard) is down
		#define GKEYSTATE_FN				(1<<GKEYSTATE_FN_BIT)			// Fn is down
		#define GKEYSTATE_COMPOSE			(1<<GKEYSTATE_COMPOSE_BIT)		// Compose is down
		#define GKEYSTATE_WINKEY			(1<<GKEYSTATE_WINKEY_BIT)		// WinKey is down
		#define GKEYSTATE_APPLE_CTRL		(1<<GKEYSTATE_WINKEY_BIT)		// Control key on apple keyboard is down
		#define GKEYSTATE_CAPSLOCK			(1<<GKEYSTATE_CAPSLOCK_BIT)		// CapsLock is on
		#define GKEYSTATE_NUMLOCK			(1<<GKEYSTATE_NUMLOCK_BIT)		// NumLock is on
		#define GKEYSTATE_SCROLLLOCK		(1<<GKEYSTATE_SCROLLLOCK_BIT)	// ScrollLock is on
		#define GKEYSTATE_LAYOUT_FIRST		(1<<GKEYSTATE_LAYOUT_FIRST_BIT)	// 4 bits for extra states for the key Layout
		#define GKEYSTATE_SYSTEM_FIRST		(1<<GKEYSTATE_SYSTEM_FIRST_BIT)	// 1st available flag for the system's use (maximum 4 bits)
		#define GKEYSTATE_DRIVER_FIRST		(1<<GKEYSTATE_DRIVER_FIRST_BIT)	// 1st available flag for the driver's use (maximum 7 bits)
		#define GKEYSTATE_MISSED_EVENT		(1<<GKEYSTATE_MISSED_EVENT_BIT)	// A keyboard event has been missed

		#define GKEYSTATE_SHIFT				(GKEYSTATE_SHIFT_L|GKEYSTATE_SHIFT_R)	// A shift key is down (left or right)
		#define GKEYSTATE_CTRL				(GKEYSTATE_CTRL_L|GKEYSTATE_CTRL_R)		// A control key is down (left or right)
		#define GKEYSTATE_ALT				(GKEYSTATE_ALT_L|GKEYSTATE_ALT_R)		// A alt key is down (left or right)
		#define GKEYSTATE_APPLE_OPTION		(GKEYSTATE_APPLE_OPTION_L|GKEYSTATE_APPLE_OPTION_R)		// A apple option key is down (left or right)
		#define GKEYSTATE_APPLE_CMD			(GKEYSTATE_APPLE_CMD_L|GKEYSTATE_APPLE_CMD_R)			// A apple cmd key is down (left or right)
} GEventKeyboard;

// Keyboard Listen Flags - passed to geventAddSourceToListener()
#define GLISTEN_KEYREPEATSOFF	0x0001			// Ignore key repeats (if possible)
#define GLISTEN_KEYNOSPECIALS	0x0002			// Ignore special keys
#define GLISTEN_KEYUP			0x0004			// Return keyup's as well as key down events
#define GLISTEN_KEYTRANSITIONS	0x0008			// Return transitions to the key state
#define GLISTEN_KEYRAW			0x0010			// Return raw scan-codes. This turns off normal character processing.

// All keyboards
#define GKEYBOARD_ALL_INSTANCES		((unsigned)-1)

#endif

/*===========================================================================*/
/* External declarations.                                                    */
/*===========================================================================*/

#ifdef __cplusplus
extern "C" {
#endif

	/**
	 * @brief	Create a keyboard input instance
	 *
	 * @param[in] instance	The ID of the keyboard input instance (from 0 to 9999)
	 *
	 * @return	The source handle of the created input instance
	 */
	GSourceHandle ginputGetKeyboard(unsigned instance);
	
	#if GINPUT_NEED_KEYBOARD || defined(__DOXYGEN__)

		/**
		 * @brief	Get the current keyboard status
		 *
		 * @param[in] instance	The ID of the keyboard input instance
		 * @param[in] pkeyboard	The keyboard event struct
		 *
		 * @return Returns FALSE on an error (eg invalid instance)
		 */
		bool_t ginputGetKeyboardStatus(unsigned instance, GEventKeyboard *pkeyboard);

		#if !GKEYBOARD_LAYOUT_OFF || defined(__DOXYGEN__)
			/**
			 * @brief	Set the keyboard layout
			 *
			 * @param[in] instance	The ID of the keyboard input instance
			 * @param[in] pLayout	The keyboard layout micro-code. Passing NULL defaults to the driver's default layout.
			 *
			 * @return Returns FALSE on an error (eg invalid instance)
			 */
			bool_t ginputSetKeyboardLayout(unsigned instance, const void *pLayout);
		#endif
	#endif /* GINPUT_NEED_KEYBOARD */

#ifdef __cplusplus
}
#endif

#endif /* _GINPUT_KEYBOARD_H */
/** @} */

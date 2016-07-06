/*
 * This file is subject to the terms of the GFX License. If a copy of
 * the license was not distributed with this file, you can obtain one at:
 *
 *              http://ugfx.org/license.html
 */

/**
 * @file    src/ginput/ginput_driver_keyboard.h
 * @brief   GINPUT LLD header file for keyboard drivers.
 *
 * @defgroup Keyboard Keyboard
 * @ingroup GINPUT
 * @{
 */

#ifndef _LLD_GINPUT_KEYBOARD_H
#define _LLD_GINPUT_KEYBOARD_H

#if GINPUT_NEED_KEYBOARD //|| defined(__DOXYGEN__)

// Include the GDRIVER infrastructure
#include "../gdriver/gdriver.h"

typedef struct GKeyboard {
	GDriver			d;					// The driver overheads and vmt
	uint16_t		cntc;				// The byte count in c
	uint16_t		cntsc;				// The byte count in sc
	char			c[8];				// The utf8 code for the current key
	char			sc[8];				// The scancode for the current key
	uint32_t		keystate;			// The keyboard state.
	uint16_t		flags;
		#define GKEYBOARD_FLG_NEEDREAD	0x0001
	uint16_t		laystate;			// The layout state.
	const uint8_t *	pLayout;			// The current keyboard layout
	// Other driver specific fields may follow.
} GKeyboard;

typedef struct GKeyboardVMT {
	GDriverVMT	d;											// Device flags are part of the general vmt
		#define GKEYBOARD_VFLG_NOPOLL			0x0001		// Do not poll this device - it is purely interrupt driven
		#define GKEYBOARD_VFLG_DYNAMICONLY		0x8000		// This keyboard driver should not be statically initialized eg Win32
	const uint8_t *	defLayout;								// The default keyboard layout
	bool_t	(*init)(GKeyboard *m, unsigned driverinstance);	// Required
	void	(*deinit)(GKeyboard *m);						// Optional
	int		(*getdata)(GKeyboard *k, uint8_t *pch, int sz);	// Required. Get zero or more scancode bytes. Returns the number of scancode bytes returns
	void	(*putdata)(GKeyboard *k, char ch);				// Optional. Send a single byte to the keyboard.
} GKeyboardVMT;

#define gkvmt(m)		((const GKeyboardVMT const *)((m)->d.vmt))

/*===========================================================================*/
/* External declarations.                                                    */
/*===========================================================================*/

// If we are not using multiple keyboards then hard-code the VMT name
#if !defined(GINPUT_KEYBOARD_DRIVER_LIST)
	#undef GKEYBOARD_DRIVER_VMT
	#define GKEYBOARD_DRIVER_VMT		GKEYBOARDVMT_OnlyOne
#endif

#ifdef __cplusplus
extern "C" {
#endif
	/**
	 * @brief	Initialize a keyboard driver
	 *
	 * @param[in] g					The keyboard driver
	 * @param[in] param				Unused by keyboard
	 * @param[in] driverinstance	The driver instance		ToDo: Add some more details
	 * @param[in] systeminstance	The mouse instance		ToDo: Add some more details
	 *
	 * @return	TRUE on success, FALSE otherwise
	 * @note	This routine is provided by the high level code for
	 * 			use in the driver VMT's GMouseVMT.d structure.
	 *
	 * @notapi
	 */
	bool_t _gkeyboardInitDriver(GDriver *g, void *param, unsigned driverinstance, unsigned systeminstance);

	/**
	 * @brief	Routine that is called after initialization
	 *
	 * @param[in] g		The keyboard driver
	 * @note	This routine is provided by the high level code for
	 * 			use in the driver VMT's GKeyboardVMT.d structure.
	 *
	 * @notapi
	 */
	void _gkeyboardPostInitDriver(GDriver *g);

	/**
	 * @brief	Deinitialize a keyboard driver
	 *
	 * @param[in] g		The kerboard driver
	 * @note	This routine is provided by the high level code for
	 * 			use in the driver VMT's GKeyboardVMT.d structure.
	 *
	 * @notapi
	 */
	void _gkeyboardDeInitDriver(GDriver *g);

	/**
	 * @brief	Wakeup the high level code so that it attempts another read
	 *
	 * @note	This routine is provided to low level drivers by the high level code
	 *
	 * @notapi
	 */
	void _gkeyboardWakeup(GKeyboard *k);

	/**
	 * @brief	Wakeup the high level code so that it attempts another read
	 *
	 * @note	This routine is provided to low level drivers by the high level code
	 *
	 * @iclass
	 * @notapi
	 */
	void _gkeyboardWakeupI(GKeyboard *k);

#ifdef __cplusplus
}
#endif

#endif /* GINPUT_NEED_KEYBOARD */

#endif /* _LLD_GINPUT_KEYBOARD_H */
/** @} */

/*
 * This file is subject to the terms of the GFX License. If a copy of
 * the license was not distributed with this file, you can obtain one at:
 *
 *              http://ugfx.org/license.html
 */

/**
 * @file    src/ginput/ginput_keyboard_microcode.c
 * @brief   GINPUT keyboard standard microcode definitions.
 */

#include "../../gfx.h"

#if GFX_USE_GINPUT && GINPUT_NEED_KEYBOARD && !GKEYBOARD_LAYOUT_OFF

#include "ginput_keyboard_microcode.h"

#if GKEYBOARD_LAYOUT_SCANCODE2_US

	#error "Keyboard Layout SCANCODE2_US is not fully implemented yet"

	uint8_t	KeyboardLayout_ScancodeSet2_US[] = {
		KMC_HEADERSTART, KMC_HEADER_ID1, KMC_HEADER_ID2, KMC_HEADER_VER_1,

		KMC_RECORDSTART, 0x03,													// Handle E0 codes (ignore for now assuming a single character)
			KMC_TEST_LASTCODE, 0xE0,
			KMC_ACT_DONE,
		KMC_RECORDSTART, 0x03,
			KMC_TEST_CODE, 0xE0,
			KMC_ACT_STOP,

		KMC_RECORDSTART, 0x03,													// Handle E1 codes (ignore for now assuming a single character)
			KMC_TEST_LASTCODE, 0xE1,
			KMC_ACT_DONE,
		KMC_RECORDSTART, 0x03,
			KMC_TEST_CODE, 0xE1,
			KMC_ACT_STOP,

		KMC_RECORDSTART, 0x03,													// Handle E2 codes (ignore for now assuming a single character)
			KMC_TEST_LASTCODE, 0xE2,
			KMC_ACT_DONE,
		KMC_RECORDSTART, 0x03,
			KMC_TEST_CODE, 0xE2,
			KMC_ACT_STOP,

		KMC_RECORDSTART, 0x06,													// KeyUp
			KMC_TEST_CODEBIT, 0x80,
			KMC_ACT_STATEBIT, GKEYSTATE_KEYUP_BIT,
			KMC_ACT_CODEBIT, 0x80 | KMC_BIT_CLEAR,

		KMC_RECORDSTART, 0x05,													// CapsLock (on keyup to avoid repeats)
			KMC_TEST_CODE, 0x58,
			KMC_TEST_STATEBIT, GKEYSTATE_KEYUP_BIT | KMC_BIT_CLEAR,
			KMC_ACT_DONE,
		KMC_RECORDSTART, 0x05,
			KMC_TEST_CODE, 0x58,
			KMC_ACT_STATEBIT, GKEYSTATE_CAPSLOCK_BIT | KMC_BIT_INVERT,
			KMC_ACT_DONE,

		KMC_RECORDSTART, 0x05,													// Detect Shift Keys
			//KMC_ACT_LAYOUTBIT, SCANCODESET2_LAYOUT_E0_BIT | KMC_BIT_CLEAR,
			KMC_ACT_STOP,

		KMC_RECORDSTART, 0x03,
			KMC_ACT_CHARRANGE, 0x00,
			KMC_ACT_DONE,

		KMC_RECORDSTART, 0x00,
	};
#endif // GKEYBOARD_LAYOUT_SCANCODE2_US

#endif // GFX_USE_GINPUT && GINPUT_NEED_KEYBOARD && !GKEYBOARD_LAYOUT_OFF

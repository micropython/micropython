/*
 * This file is subject to the terms of the GFX License. If a copy of
 * the license was not distributed with this file, you can obtain one at:
 *
 *              http://ugfx.org/license.html
 */

#include "../../gfx.h"

#if GFX_USE_GWIN && GWIN_NEED_KEYBOARD

#include "gwin_keyboard_layout.h"

#if GWIN_NEED_KEYBOARD_ENGLISH1
	/* For this keyboard mapping we use:
	 * Set 0 = Lowercase letters
	 * Set 1 = Uppercase letters (transient)
	 * Set 2 = Uppercase letters (locked)
	 * Set 3 = Numbers
	 * Set 4 = Symbols
	 */
	static const GVSpecialKey Eng1SKeys[] = {
			{ "\001", 0, GVKEY_SINGLESET, 1 },				// \001 (1)	= Shift Lower to Upper
			{ "\001", 0, GVKEY_INVERT|GVKEY_LOCKSET, 2 },	// \002 (2)	= Shift Upper to Upper Lock
			{ "\002", 0, GVKEY_INVERT|GVKEY_LOCKSET, 0 },	// \003 (3)	= Shift Upper Lock to Lower
			{ "123", 0, GVKEY_LOCKSET, 3 },					// \004 (4)	= Change to Numbers
			{ "\010", "\b", 0, 0 },							// \005 (5)	= Backspace
			{ "\015", "\r", 0, 0 },							// \006 (6)	= Enter 1
			{ "\015", "\r", 0, 0 },							// \007 (7)	= Enter 2 (Short keycap)
			{ "Sym", 0, GVKEY_LOCKSET, 4 },					// \010 (8)	= Change to Symbols
			{ "aA", 0, GVKEY_LOCKSET, 0 },					// \011 (9)	= Change to Lower Alpha
	};
	static const char Eng1Set0Row3[] = "\004  .\006\006";
	static const char Eng1Set1Row0[] = "QWERTYUIOP";
	static const char Eng1Set1Row1[] = "ASDFGHJKL";
	static const char *Eng1Set0[] = { "qwertyuiop",	"asdfghjkl",	"\001zxcvbnm\005",	Eng1Set0Row3,		0 };
	static const char *Eng1Set1[] = { Eng1Set1Row0,	Eng1Set1Row1,	"\002ZXCVBNM\005",	Eng1Set0Row3,		0 };
	static const char *Eng1Set2[] = { Eng1Set1Row0,	Eng1Set1Row1,	"\003ZXCVBNM\005",	Eng1Set0Row3,		0 };
	static const char *Eng1Set3[] = { "+-*/",		"@789",			"\007456",		"\010123",			"\01100.",	0 };
	static const char *Eng1Set4[] = { "#$%^&*()",	"~`:;\"'{}",	"<>?/\\|[]",	"\011\004,!  .@",	0 };
	static const GVKeySet Eng1Sets[] = { Eng1Set0, Eng1Set1, Eng1Set2, Eng1Set3, Eng1Set4, 0 };
	const GVKeyTable VirtualKeyboard_English1 = { Eng1SKeys, Eng1Sets };
#endif // GWIN_NEED_KEYBOARD_ENGLISH1

#endif // GFX_USE_GWIN && GWIN_NEED_KEYBOARD

/*
 * This file is subject to the terms of the GFX License. If a copy of
 * the license was not distributed with this file, you can obtain one at:
 *
 *              http://ugfx.org/license.html
 */

/**
 * @file    src/ginput/ginput_keyboard_microcode.h
 * @brief   GINPUT keyboard layout microcode definition.
 */

#ifndef _KEYBOARD_MICROCODE_H
#define _KEYBOARD_MICROCODE_H

/*
 * Keyboard Layout Microcode Definition
 *
 * Each time a byte is received from the keyboard it is processed through the layout microcode engine. This enables conversion from
 * 	scancodes to ascii, internationalization, and various other tricky keyboard behavior.
 * Note a "code" is defined as a single byte of data from the keyboard, a "scancode" is one or more "codes" that are sent in response to one keyboard press or release.
 *
 * The layout microcode can even be switched on the fly by the application to effect such changes as changing from US English to Russian layouts.
 *	They could conceivably even be loaded from disk at run-time.
 *
 * In the interest of efficiency there is very little error checking. Make sure your layout microcode has been debugged properly before releasing
 * 	to production code.
 *
 * Layout microcode consists of a header followed by 1 or more records.
 *
 * The header consists of a KMC_HEADERSTART and associated bytes. The is used only to check it looks like it might be layout microcode and to specify the
 * 	version of microcode. Future versions of layout microcode will always have the same header at least to the version number.
 *
 * Each record is delimited by a KMC_RECORDSTART. Each record can contain a maximum of 255 bytes.
 * 	A record length of zero indicates the end of the layout microcode.
 *
 * A record consists a mixture of tests and actions (normally the tests are first). If a test fails the rest of this record is skipped and the next
 * 	record is processed. This has the effect of AND'ing multiple tests that occur together.
 * 	KMC_TEST_INIT and KMC_TEST_ERROR are special. These tests must be the first byte in their respective record as without this test being there, there
 * 	is an implicit test that a code has actually been received.
 * 	If no records have successful tests for this code then by extension no actions are executed. That is, the code is ignored.
 * 	After fully processing a record, the next record is processed. This can be prevented by using the KMC_ACT_STOP action. When encountered all processing
 * 	on this code stops.
 *
 * Some tests set a pseudo variable called "diff". This is then used by some actions. At the start of a new record "diff" is set to "code" (or 0 for the init and
 * 	error conditions).
 * Some tests and actions do bit operations on either the saved key-state or on the code itself. Bit numbers (which can range from 0 to 31) test or affect the
 * 	"set" state of the bit. OR'ing KMC_BIT_CLEAR with the bit number test or affect the "clear" state of the bit. For example, KMC_ACT_STATEBIT with a parameter
 * 	of 10 will set bit 10 of the key-state. KMC_ACT_STATEBIT with a parameter of (10 | KMC_BIT_CLEAR) will clear bit 10 of the key-state.
 *
 */

#define KMC_HEADERSTART		0x00	// Followed by:	ID1 ID2 VER		- This is the start of layout microcode.
	#define KMC_HEADER_ID1		'L'
	#define KMC_HEADER_ID2		'M'
	#define KMC_HEADER_VER_1	0x01

	#define KMC_HEADER_VER_CURRENT	KMC_HEADER_VER_1	// The current version number
	#define KMC_HEADER_VER_MIN		KMC_HEADER_VER_1	// The minimum version number accepted
	#define KMC_HEADER_VER_MAX		KMC_HEADER_VER_1	// The maximum version number accepted

#define KMC_RECORDSTART		0x01	// Followed by:	nn b0 b1 ... b(nn-1)	- nn bytes of test and action follow, nn = 00 means end of all tests

#define KMC_TEST_INIT		0x10	// Followed by:	nothing			- The layout is initializing
#define KMC_TEST_ERROR		0x11	// Followed by:	nothing			- The keyboard has signaled an error
#define KMC_TEST_CODE		0x12	// Followed by:	aa				- Code must equal aa. Diff is set to 0
#define KMC_TEST_CODERANGE	0x13	// Followed by:	aa bb			- Code must be between aa and bb (inclusive). Diff is set to (code - aa)
#define KMC_TEST_CODETABLE	0x14	// Followed by:	n m1 m2 ...		- Code must equal an m value. There are n possible m values. Diff is set to 0 to n-1 (the match position)
#define KMC_TEST_STATEBIT	0x15	// Followed by:	b				- Test if a key-state bit is set/clear. b = 0 to 31 or b = (0 | KMC_BIT_CLEAR) to (31 | KMC_BIT_CLEAR)
#define KMC_TEST_STATEOR	0x16	// Followed by:	b1 b2			- Test two key-state bits and OR the result
#define KMC_TEST_STATEAND	0x17	// Followed by:	b1 b2			- Test two key-state bits and AND the result
#define KMC_TEST_LAYOUTBIT	0x18	// Followed by:	b				- Test if a layout bit is set/clear. b = 0 to 15 or b = (0 | KMC_BIT_CLEAR) to (15 | KMC_BIT_CLEAR)
#define KMC_TEST_LAYOUTOR	0x19	// Followed by:	b1 b2			- Test two layout bits and OR the result
#define KMC_TEST_LAYOUTAND	0x1A	// Followed by:	b1 b2			- Test two layout bits and AND the result
#define KMC_TEST_CODEBIT	0x1B	// Followed by:	b				- Test if a code bit is set/clear. b = 0 to 7 or b = (0 | KMC_BIT_CLEAR) to (7 | KMC_BIT_CLEAR)
#define KMC_TEST_CODEOR		0x1C	// Followed by:	b1 b2			- Test two code bits and OR the result
#define KMC_TEST_CODEAND	0x1D	// Followed by:	b1 b2			- Test two code bits and AND the result
#define KMC_TEST_LASTCODE	0x1E	// Followed by:	aa				- Test if the last scancode was aa
#define KMC_TEST_SHIFT		0x20	// Followed by: nothing			- Test if a shift key is down
#define KMC_TEST_NOSHIFT	0x21	// Followed by: nothing			- Test if a shift key is not down
#define KMC_TEST_CTRL		0x22	// Followed by: nothing			- Test if a control key is down
#define KMC_TEST_NOCTRL		0x23	// Followed by: nothing			- Test if a control key is not down
#define KMC_TEST_ALT		0x24	// Followed by: nothing			- Test if an alt key is down
#define KMC_TEST_NOALT		0x25	// Followed by: nothing			- Test if an alt key is not down
#define KMC_TEST_CAPS		0x26	// Followed by: nothing			- Test if capslock as modified by shift is active
#define KMC_TEST_NOCAPS		0x27	// Followed by: nothing			- Test if capslock as modified by shift is not active
#define KMC_TEST_NUMLOCK	0x28	// Followed by: nothing			- Test if numlock is active
#define KMC_TEST_NONUMLOCK	0x29	// Followed by: nothing			- Test if numlock is not active

#define KMC_ACT_STOP		0xFF	// Followed by:	nothing			- Stop processing this code
#define KMC_ACT_DONE		0xFE	// Followed by:	nothing			- Finished processing this scancode sequence. (also implies STOP)
#define KMC_ACT_RESET		0xFD	// Followed by:	nothing			- Empty all buffers
#define KMC_ACT_STATEBIT	0x80	// Followed by:	b				- Set or clear bit b in key-state. b = 0 to 31 or b = (0 | KMC_BIT_CLEAR) to (31 | KMC_BIT_CLEAR)
#define KMC_ACT_LAYOUTBIT	0x81	// Followed by:	b				- Set or clear bit b in layout bits. b = 0 to 15 or b = (0 | KMC_BIT_CLEAR) to (15 | KMC_BIT_CLEAR)
#define KMC_ACT_CODEBIT		0x82	// Followed by:	b				- Set or clear bit b in code. b = 0 to 7 or b = (0 | KMC_BIT_CLEAR) to (7 | KMC_BIT_CLEAR)
#define KMC_ACT_CHAR		0x83	// Followed by:	nn				- Append char nn to output buffer
#define KMC_ACT_CHARCODE	0x84	// Followed by:	nothing			- Append the code to output buffer
#define KMC_ACT_CHARRANGE	0x85	// Followed by:	nn				- Append char nn + Diff to output
#define KMC_ACT_CHARTABLE	0x86	// Followed by:	n c1 c2 ...		- Append char to output based on c[Diff]. If Diff is greater than n then nothing is appended.
#define KMC_ACT_CLEAR		0x87	// Followed by: nothing			- Clear the output buffer
#define KMC_ACT_CHARADD		0x88	// Followed by:	nn				- Multiple the last char in output buffer by nn (assume 0 if none) and add Diff
#define KMC_ACT_DATA		0x89	// Followed by:	nn				- Send nn back to the keyboard

#define KMC_BIT_CLEAR		0x80	// The bit number is being used for a bit clear operation rather than a bit set operation.
#define KMC_BIT_INVERT		0x40	// Invert the bit rather than setting or clearing it.

#endif /* _KEYBOARD_MICROCODE_H */

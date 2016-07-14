/*
 * This file is subject to the terms of the GFX License. If a copy of
 * the license was not distributed with this file, you can obtain one at:
 *
 *              http://ugfx.org/license.html
 */

/**
 * @file    src/ginput/ginput_keyboard.c
 * @brief   GINPUT keyboard code.
 */

// We need to include stdio.h below for MICROCODE_DEBUG. Turn off GFILE_NEED_STDIO just for this file to prevent conflicts
#define GFILE_NEED_STDIO_MUST_BE_OFF

#include "../../gfx.h"

#if GFX_USE_GINPUT && GINPUT_NEED_KEYBOARD

#define MICROCODE_DEBUG		FALSE

#if MICROCODE_DEBUG
	#include <stdio.h>
#endif

// Get the keyboard driver interface
#include "ginput_driver_keyboard.h"
#include "ginput_keyboard_microcode.h"

// The keyboard poll timer
static GTIMER_DECL(KeyboardTimer);

static void SendKeyboardEventToListener(GSourceListener	*psl, GKeyboard *k) {
	GEventKeyboard		*pe;
	int					i;

	// If there is no event buffer just mark a missed event
	if (!(pe = (GEventKeyboard *)geventGetEventBuffer(psl))) {
		// This listener is missing - save the meta events that have happened
		psl->srcflags |= GKEYSTATE_MISSED_EVENT;
		return;
	}

	if ((psl->listenflags & GLISTEN_KEYRAW)) {
		pe->type = GEVENT_KEYBOARD;
		pe->bytecount = k->cntsc;
		for(i = 0; i < k->cntsc; i++)	pe->c[i] = k->sc[i];
		for(; i < 8; i++)				pe->c[i] = 0;
		pe->keystate = k->keystate | psl->srcflags | GKEYSTATE_RAW;
		psl->srcflags = 0;
		return;
	}

	if ((psl->listenflags & GLISTEN_KEYREPEATSOFF) && (k->keystate & GKEYSTATE_REPEAT))
		return;

	if ((psl->listenflags & GLISTEN_KEYNOSPECIALS) && (k->keystate & GKEYSTATE_SPECIAL))
		return;

	if (!(psl->listenflags & GLISTEN_KEYUP) && (k->keystate & GKEYSTATE_KEYUP))
		k->cntc = 0;

	if (!(psl->listenflags & GLISTEN_KEYTRANSITIONS) && !k->cntc)
		return;

	pe->type = GEVENT_KEYBOARD;
	pe->bytecount = k->cntc;
	for(i = 0; i < k->cntc; i++)	pe->c[i] = k->c[i];
	for(; i < 8; i++)				pe->c[i] = 0;
	pe->keystate = k->keystate | psl->srcflags;
	psl->srcflags = 0;
	geventSendEvent(psl);
}

static void SendKeyboardEvent(GKeyboard *k) {
	GSourceListener	*psl;

	// Send to the "All Keyboards" source listeners
	psl = 0;
	while ((psl = geventGetSourceListener((GSourceHandle)&KeyboardTimer, psl)))
		SendKeyboardEventToListener(psl, k);

	// Send to the keyboard specific source listeners
	psl = 0;
	while ((psl = geventGetSourceListener((GSourceHandle)k, psl)))
		SendKeyboardEventToListener(psl, k);
}

#define FLAG_ERROR		0x01
#define FLAG_INIT		0x02

#if GKEYBOARD_LAYOUT_OFF
	static void microengine(GKeyboard *k, uint8_t code, uint8_t flags) {
		if (flags)
			return;

		// Just send an event using the char
		k->c[0] = k->sc[0] = code;
		k->cntc = k->cntsc = 1;
		SendKeyboardEvent(k);
		k->cntc = k->cntsc = 0;
	}
#else
	static void microengine(GKeyboard *k, uint8_t code, uint8_t flags) {
		const uint8_t	*pc;
		const uint8_t	*nrec;
		uint8_t			ver, diff, p1, p2;
		#if MICROCODE_DEBUG
			unsigned	cnt;
		#endif

		pc = k->pLayout;
		if (!pc) {
			if (flags)
				return;

			// Default is to just send an event using the char
			k->c[0] = k->sc[0] = code;
			k->cntc = k->cntsc = 1;
			SendKeyboardEvent(k);
			k->cntc = k->cntsc = 0;
			return;
		}

		// Check the layout header
		if (*pc++ != KMC_HEADERSTART || *pc++ != KMC_HEADER_ID1 || *pc++ != KMC_HEADER_ID2)
			return;

		// We only understand version 1 currently
		ver = *pc++;
		if (ver < KMC_HEADER_VER_MIN || ver > KMC_HEADER_VER_MAX)
			return;

		// Setup
		diff = code;
		if (k->cntsc >= sizeof(k->sc))
			flags |= FLAG_ERROR;
		else
			k->sc[k->cntsc++] = code;

		#if MICROCODE_DEBUG
			cnt = 0;
		#endif

		while(*pc++ == KMC_RECORDSTART) {
			// Get the record length
			p1 = *pc++;
			if (!p1) break;
			nrec = pc + p1;

			#if MICROCODE_DEBUG
				cnt++;
			#endif

			while(pc < nrec) {
				switch(*pc++) {
				case KMC_TEST_INIT:
					if (!(flags & FLAG_INIT))  goto nextrecord;
					break;
				case KMC_TEST_ERROR:
					if (!(flags & FLAG_ERROR)) goto nextrecord;
					break;
				case KMC_TEST_CODE:
					if (flags != 0) goto nextrecord;
					p1 = *pc++;
					if (p1 != code) goto nextrecord;
					diff = 0;
					break;
				case KMC_TEST_CODERANGE:
					if (flags != 0) goto nextrecord;
					p1 = *pc++;
					p2 = *pc++;
					if (code < p1 || code > p2) goto nextrecord;
					diff = code - p1;
					break;
				case KMC_TEST_CODETABLE:
					if (flags != 0) goto nextrecord;
					p1 = *pc++;
					for(p2 = 0; ; p2++, p1--, pc++) {
						if (!p1) goto nextrecord;
						if (*pc == code) break;
					}
					pc += p1;
					diff = p2;
					break;
				case KMC_TEST_STATEBIT:
					p1 = *pc++;
					if ((p1 & KMC_BIT_CLEAR)) {
						if ((k->keystate & (1 << (p1 & 31)))) goto nextrecord;
					} else {
						if (!(k->keystate & (1 << (p1 & 31)))) goto nextrecord;
					}
					break;
				case KMC_TEST_STATEOR:
					p1 = *pc++;
					if ((p1 & KMC_BIT_CLEAR)) {
						if (!(k->keystate & (1 << (p1 & 31)))) break;
					} else {
						if ((k->keystate & (1 << (p1 & 31)))) break;
					}
					p2 = *pc++;
					if ((p2 & KMC_BIT_CLEAR)) {
						if (!(k->keystate & (1 << (p2 & 31)))) break;
					} else {
						if ((k->keystate & (1 << (p2 & 31)))) break;
					}
					goto nextrecord;
				case KMC_TEST_STATEAND:
					p1 = *pc++;
					if ((p1 & KMC_BIT_CLEAR)) {
						if ((k->keystate & (1 << (p1 & 31)))) goto nextrecord;
					} else {
						if (!(k->keystate & (1 << (p1 & 31)))) goto nextrecord;
					}
					p2 = *pc++;
					if ((p2 & KMC_BIT_CLEAR)) {
						if ((k->keystate & (1 << (p2 & 31)))) goto nextrecord;
					} else {
						if (!(k->keystate & (1 << (p2 & 31)))) goto nextrecord;
					}
					break;
				case KMC_TEST_LAYOUTBIT:
					p1 = *pc++;
					if ((p1 & KMC_BIT_CLEAR)) {
						if ((k->laystate & (1 << (p1 & 15)))) goto nextrecord;
					} else {
						if (!(k->laystate & (1 << (p1 & 15)))) goto nextrecord;
					}
					break;
				case KMC_TEST_LAYOUTOR:
					p1 = *pc++;
					if ((p1 & KMC_BIT_CLEAR)) {
						if (!(k->laystate & (1 << (p1 & 15)))) break;
					} else {
						if ((k->laystate & (1 << (p1 & 15)))) break;
					}
					p2 = *pc++;
					if ((p2 & KMC_BIT_CLEAR)) {
						if (!(k->laystate & (1 << (p2 & 15)))) break;
					} else {
						if ((k->laystate & (1 << (p2 & 15)))) break;
					}
					goto nextrecord;
				case KMC_TEST_LAYOUTAND:
					p1 = *pc++;
					if ((p1 & KMC_BIT_CLEAR)) {
						if ((k->laystate & (1 << (p1 & 15)))) goto nextrecord;
					} else {
						if (!(k->laystate & (1 << (p1 & 15)))) goto nextrecord;
					}
					p2 = *pc++;
					if ((p2 & KMC_BIT_CLEAR)) {
						if ((k->laystate & (1 << (p2 & 15)))) goto nextrecord;
					} else {
						if (!(k->laystate & (1 << (p2 & 15)))) goto nextrecord;
					}
					break;
				case KMC_TEST_CODEBIT:
					if (flags != 0) goto nextrecord;
					p1 = *pc++;
					if ((p1 & KMC_BIT_CLEAR)) {
						if ((code & (1 << (p1 & 7)))) goto nextrecord;
					} else {
						if (!(code & (1 << (p1 & 7)))) goto nextrecord;
					}
					break;
				case KMC_TEST_CODEOR:
					if (flags != 0) goto nextrecord;
					p1 = *pc++;
					if ((p1 & KMC_BIT_CLEAR)) {
						if (!(code & (1 << (p1 & 7)))) break;
					} else {
						if ((code & (1 << (p1 & 7)))) break;
					}
					p2 = *pc++;
					if ((p2 & KMC_BIT_CLEAR)) {
						if (!(code & (1 << (p2 & 7)))) break;
					} else {
						if ((code & (1 << (p2 & 7)))) break;
					}
					goto nextrecord;
				case KMC_TEST_CODEAND:
					if (flags != 0) goto nextrecord;
					p1 = *pc++;
					if ((p1 & KMC_BIT_CLEAR)) {
						if ((code & (1 << (p1 & 7)))) goto nextrecord;
					} else {
						if (!(code & (1 << (p1 & 7)))) goto nextrecord;
					}
					p2 = *pc++;
					if ((p2 & KMC_BIT_CLEAR)) {
						if ((code & (1 << (p2 & 7)))) goto nextrecord;
					} else {
						if (!(code & (1 << (p2 & 7)))) goto nextrecord;
					}
					break;
				case KMC_TEST_LASTCODE:
					p1 = *pc++;
					if (k->cntsc < 2) goto nextrecord;
					if (p1 != k->sc[k->cntsc-2]) goto nextrecord;
					break;
				case KMC_TEST_SHIFT:
					if ((k->keystate & (GKEYSTATE_SHIFT_L|GKEYSTATE_SHIFT_R))) break;
					goto nextrecord;
				case KMC_TEST_NOSHIFT:
					if (!(k->keystate & (GKEYSTATE_SHIFT_L|GKEYSTATE_SHIFT_R))) break;
					goto nextrecord;
				case KMC_TEST_CTRL:
					if ((k->keystate & (GKEYSTATE_CTRL_L|GKEYSTATE_CTRL_R))) break;
					goto nextrecord;
				case KMC_TEST_NOCTRL:
					if (!(k->keystate & (GKEYSTATE_CTRL_L|GKEYSTATE_CTRL_R))) break;
					goto nextrecord;
				case KMC_TEST_ALT:
					if ((k->keystate & (GKEYSTATE_ALT_L|GKEYSTATE_ALT_R))) break;
					goto nextrecord;
				case KMC_TEST_NOALT:
					if (!(k->keystate & (GKEYSTATE_ALT_L|GKEYSTATE_ALT_R))) break;
					goto nextrecord;
				case KMC_TEST_CAPS:
					if ((k->keystate & GKEYSTATE_CAPSLOCK)) {
						if (!(k->keystate & (GKEYSTATE_SHIFT_L|GKEYSTATE_SHIFT_R))) break;
					} else {
						if ((k->keystate & (GKEYSTATE_SHIFT_L|GKEYSTATE_SHIFT_R))) break;
					}
					goto nextrecord;
				case KMC_TEST_NOCAPS:
					if ((k->keystate & GKEYSTATE_CAPSLOCK)) {
						if ((k->keystate & (GKEYSTATE_SHIFT_L|GKEYSTATE_SHIFT_R))) break;
					} else {
						if (!(k->keystate & (GKEYSTATE_SHIFT_L|GKEYSTATE_SHIFT_R))) break;
					}
					goto nextrecord;
				case KMC_TEST_NUMLOCK:
					if ((k->keystate & GKEYSTATE_NUMLOCK)) break;
					goto nextrecord;
				case KMC_TEST_NONUMLOCK:
					if (!(k->keystate & GKEYSTATE_NUMLOCK)) break;
					goto nextrecord;

				case KMC_ACT_STOP:
					#if MICROCODE_DEBUG
						fprintf(stderr, "Executed STOP:  Records=%2u Flags=0x%02X Code=0x%02X\n", cnt, flags, code); fflush(stderr);
					#endif
					return;
				case KMC_ACT_DONE:
					SendKeyboardEvent(k);
					k->cntc = k->cntsc = 0;
					k->keystate &= ~(GKEYSTATE_KEYUP|GKEYSTATE_SPECIAL);
					#if MICROCODE_DEBUG
						fprintf(stderr, "Executed DONE:  Records=%2u Flags=0x%02X Code=0x%02X\n", cnt, flags, code); fflush(stderr);
					#endif
					return;
				case KMC_ACT_RESET:
					k->cntc = k->cntsc = 0;
					k->keystate &= ~(GKEYSTATE_KEYUP|GKEYSTATE_SPECIAL);
					break;
				case KMC_ACT_STATEBIT:
					p1 = *pc++;
					if ((p1 & KMC_BIT_INVERT))
						k->keystate ^= (1 << (p1 & 31));
					else if ((p1 & KMC_BIT_CLEAR))
						k->keystate &= ~(1 << (p1 & 31));
					else
						k->keystate |= (1 << (p1 & 31));
					break;
				case KMC_ACT_LAYOUTBIT:
					p1 = *pc++;
					if ((p1 & KMC_BIT_INVERT))
						k->laystate ^= (1 << (p1 & 15));
					else if ((p1 & KMC_BIT_CLEAR))
						k->laystate &= ~(1 << (p1 & 15));
					else
						k->laystate |= (1 << (p1 & 15));
					break;
				case KMC_ACT_CODEBIT:
					p1 = *pc++;
					if ((p1 & KMC_BIT_INVERT))
						code ^= (1 << (p1 & 7));
					else if ((p1 & KMC_BIT_CLEAR))
						code &= ~(1 << (p1 & 7));
					else
						code |= (1 << (p1 & 7));
					break;
				case KMC_ACT_CHAR:
					if (k->cntc >= sizeof(k->c)) goto codeerror;
					k->c[k->cntc++] = *pc++;
					break;
				case KMC_ACT_CHARCODE:
					if (k->cntc >= sizeof(k->c)) goto codeerror;
					k->c[k->cntc++] = code;
					break;
				case KMC_ACT_CHARRANGE:
					if (k->cntc >= sizeof(k->c)) goto codeerror;
					k->c[k->cntc++] = diff + *pc++;
					break;
				case KMC_ACT_CHARTABLE:
					p1 = *pc++;
					if (diff < p1) {
						if (k->cntc >= sizeof(k->c)) goto codeerror;
						k->c[k->cntc++] = pc[diff];
					}
					pc += p1;
					break;
				case KMC_ACT_CLEAR:
					k->cntc = 0;
					break;
				case KMC_ACT_CHARADD:
					p1 = *pc++;
					if (!k->cntc)
						k->c[k->cntc++] = 0;
					k->c[k->cntc-1] = k->c[k->cntc-1] * p1 + diff;
					break;
				case KMC_ACT_DATA:
					p1 = *pc++;
					if (gkvmt(k)->putdata)
						gkvmt(k)->putdata(k, p1);
					break;

				default:
				codeerror:
					#if MICROCODE_DEBUG
						fprintf(stderr, "Executed ERROR: Records=%2u Flags=0x%02X Code=0x%02X\n", cnt, flags, code);	cnt = 0; fflush(stderr);
					#endif

					// Prevent recursion
					if (flags & FLAG_ERROR)
						return;

					// Process as an error
					flags |= FLAG_ERROR;
					nrec = k->pLayout + 4;			// Jump back to the end of the header to process the error
					goto nextrecord;				// Nothing left to do here.
				}
			}

		nextrecord:
			pc = nrec;
		}

		#if MICROCODE_DEBUG
			fprintf(stderr, "Executed END:   Records=%2u Flags=0x%02X Code=0x%02X\n", cnt, flags, code); fflush(stderr);
		#endif
	}
#endif

static void KeyboardPoll(void *param) {
	GKeyboard *	k;
	uint8_t		scancodes[8];
	int			sz, i;
	(void) 		param;

	for(k = (GKeyboard *)gdriverGetNext(GDRIVER_TYPE_KEYBOARD, 0); k; k = (GKeyboard *)gdriverGetNext(GDRIVER_TYPE_KEYBOARD, (GDriver *)k)) {
		if (!(gkvmt(k)->d.flags & GKEYBOARD_VFLG_NOPOLL) || (k->flags & GKEYBOARD_FLG_NEEDREAD)) {
			k->flags &= ~GKEYBOARD_FLG_NEEDREAD;
			sz = gkvmt(k)->getdata(k, scancodes, sizeof(scancodes));
			for(i = 0; i < sz; i++)
				microengine(k, scancodes[i], 0);
		}
	}
}

void _gkeyboardInit(void) {
	// GINPUT_KEYBOARD_DRIVER_LIST is defined - create each driver instance
	#if defined(GINPUT_KEYBOARD_DRIVER_LIST)
		{
			int		i;
			typedef const GKeyboardVMT const GKEYBOARDVMTLIST[1];

			extern GKEYBOARDVMTLIST GINPUT_KEYBOARD_DRIVER_LIST;
			static const GKeyboardVMT *	const dclist[] = {GINPUT_KEYBOARD_DRIVER_LIST};

			for(i = 0; i < sizeof(dclist)/sizeof(dclist[0]); i++) {
                if (!(dclist[i]->d.flags & GKEYBOARD_VFLG_DYNAMICONLY))
					gdriverRegister(&dclist[i]->d, 0);
			}
		}

	// One and only one mouse
	#else
		{
			extern const GKeyboardVMT const GKEYBOARDVMT_OnlyOne[1];

            if (!(GKEYBOARDVMT_OnlyOne->d.flags & GKEYBOARD_VFLG_DYNAMICONLY))
					gdriverRegister(&GKEYBOARDVMT_OnlyOne->d, 0);
		}
	#endif

}

void _gkeyboardDeinit(void) {
	gtimerDeinit(&KeyboardTimer);
}

bool_t _gkeyboardInitDriver(GDriver *g, void *param, unsigned driverinstance, unsigned systeminstance) {
    #define k   ((GKeyboard *)g)
	(void) param;
    (void) systeminstance;

	// The initial keyboard layout comes from the VMT
	k->pLayout = gkvmt(k)->defLayout;

	// Init the mouse
    if (!gkvmt(k)->init((GKeyboard *)g, driverinstance))
        return FALSE;

	// Ensure the Poll timer is started
	if (!gtimerIsActive(&KeyboardTimer))
		gtimerStart(&KeyboardTimer, KeyboardPoll, 0, TRUE, GINPUT_KEYBOARD_POLL_PERIOD);

    return TRUE;

    #undef k
}

void _gkeyboardPostInitDriver(GDriver *g) {
    #define     k   ((GKeyboard *)g)

	// Run the init sequence from the layout microcode.
	microengine(k, 0, FLAG_INIT);

    #undef k
}

void _gkeyboardDeInitDriver(GDriver *g) {
    (void) g;
}

GSourceHandle ginputGetKeyboard(unsigned instance) {
	if (instance == GKEYBOARD_ALL_INSTANCES)
		return (GSourceHandle)&KeyboardTimer;
    return (GSourceHandle)gdriverGetInstance(GDRIVER_TYPE_KEYBOARD, instance);
}

bool_t ginputGetKeyboardStatus(unsigned instance, GEventKeyboard *pe) {
	GKeyboard *k;

	// Win32 threads don't seem to recognise priority and/or pre-emption
	// so we add a sleep here to prevent 100% polled applications from locking up.
	gfxSleepMilliseconds(1);

	if (!(k = (GKeyboard *)gdriverGetInstance(GDRIVER_TYPE_KEYBOARD, instance)))
		return FALSE;

	pe->type = GEVENT_KEYBOARD;
	// TODO
	return TRUE;
}

#if !GKEYBOARD_LAYOUT_OFF
	bool_t ginputSetKeyboardLayout(unsigned instance, const void *pLayout) {
		GKeyboard *k;

		if (!(k = (GKeyboard *)gdriverGetInstance(GDRIVER_TYPE_KEYBOARD, instance)))
			return FALSE;

		if (pLayout)
			k->pLayout = pLayout;
		else
			k->pLayout = gkvmt(k)->defLayout;

		return TRUE;
	}
#endif

/* Wake up the keyboard driver from an interrupt service routine (there may be new readings available) */
void _gkeyboardWakeup(GKeyboard *k) {
	if (k)
		k->flags |= GKEYBOARD_FLG_NEEDREAD;
	gtimerJab(&KeyboardTimer);
}

/* Wake up the keyboard driver from an interrupt service routine (there may be new readings available) */
void _gkeyboardWakeupI(GKeyboard *k) {
	if (k)
		k->flags |= GKEYBOARD_FLG_NEEDREAD;
	gtimerJabI(&KeyboardTimer);
}

#endif /* GFX_USE_GINPUT && GINPUT_NEED_KEYBOARD */

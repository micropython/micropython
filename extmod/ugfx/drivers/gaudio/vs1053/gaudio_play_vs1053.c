/*
 * This file is subject to the terms of the GFX License. If a copy of
 * the license was not distributed with this file, you can obtain one at:
 *
 *              http://ugfx.org/license.html
 */

#include "gfx.h"

#if GFX_USE_GAUDIO && GAUDIO_NEED_PLAY

/* Include the driver defines */
#include "../../../src/gaudio/gaudio_driver_play.h"

/* Include the vs1053 registers */
#include "vs1053.h"

/* Include the board interface */
#include "gaudio_play_board.h"

// Override-able parameters
#ifndef VS1053_CLK
	#define VS1053_CLK		12288000
#endif
#ifndef VS1053_FIRMWARE_PATCH
	#define VS1053_FIRMWARE_PATCH		FALSE
#endif
#ifndef VS1053_POLL_RATE
	#define	VS1053_POLL_RATE	5
#endif

// Load the patch file if desired. New format patches only.
#if VS1053_FIRMWARE_PATCH
	#define SKIP_PLUGIN_VARNAME
	static const uint16_t plugin[] = { /* Compressed plugin */
		#include "vs1053_patch.plg"
	};
#endif

// Set various stuff based on the clock
#if VS1053_CLK >= 16192000
	#define	SCI_MODE_STARTUP	(SCI_MODE_DEFAULTS|SM_CLK_RANGE)
	#define VS1053_CLKI			(VS1053_CLK/2)
#else
	#define	SCI_MODE_STARTUP	(SCI_MODE_DEFAULTS)
	#define VS1053_CLKI			(VS1053_CLK)
#endif
#if VS1053_CLKI > 13000000 || VS1053_CLKI < 12000000
	#error "GAUDIO(vs1053): VS1053_CLK is out of range"
#endif
#if VS1053_CLKI == 12288000
	#define	SC_FREQ_ADJUST		0x0000
#else
	#define	SC_FREQ_ADJUST		((VS1053_CLKI-8000000)/4000)
#endif
#define VS1053_MAX_SAMPLE_RATE	(VS1053_CLKI/256)
#if VS1053_CLKI > 1228800
	#define SC_CLOCK_MODE		(SC_MULT_3|SC_ADD_1|SC_FREQ_ADJUST)
#else
	#define SC_CLOCK_MODE		(SC_MULT_3_5|SC_ADD_1|SC_FREQ_ADJUST)
#endif

// Our static variables
static bool_t	vs1053_isinit;
static GTimer	playTimer;

// Some common macro's
#define waitforready()		while(!board_dreq()) gfxSleepMilliseconds(5)

static void cmd_write(uint16_t addr, uint16_t data) {
	char	buf[4];
	buf[0] = 2;
	buf[1] = (char)addr;
	buf[2] = (char)(data >> 8);
	buf[3] = (char)data;

	waitforready();
	board_startcmdwrite();
	board_spiwrite(buf, 4);
	board_endcmdwrite();
}

#if VS1053_CLK > 12288000
	static GFXINLINE void cmd_writenodreq(uint16_t addr, uint16_t data) {
		uint8_t	buf[4];

		// This is the same as cmd_write() except for it doesn't wait for dreq first
		buf[0] = 2;
		buf[1] = (uint8_t)addr;
		buf[2] = (uint8_t)(data >> 8);
		buf[3] = (uint8_t)data;

		board_startcmdwrite();
		board_spiwrite(buf, 4);
		board_endcmdwrite();
	}
#endif

static uint16_t cmd_read(uint16_t addr) {
	uint8_t	buf[2];

	buf[0] = 3;
	buf[1] = (char)addr;

	board_startcmdread();
	board_spiwrite(buf, 2);
	board_spiread(buf, 2);
	board_endcmdread();
	return (((uint16_t)buf[0])<<8)|buf[1];
}

static void data_write(const uint8_t *data, unsigned len) {
	board_startdatawrite();
	board_spiwrite(data, len);
	board_enddatawrite();
}

#if VS1053_FIRMWARE_PATCH
	static void LoadUserCode(void) {
		int			i;
		uint16_t	addr, n, val;

		for(i=0; i<sizeof(plugin)/sizeof(plugin[0]);) {
			addr = plugin[i++];
			n = plugin[i++];
			if (n & 0x8000U) { /* RLE run, replicate n samples */
				n &= 0x7FFF;
				val = plugin[i++];
				while (n--)
					cmd_write(addr, val);
			} else {           /* Copy run, copy n samples */
				while (n--)
					cmd_write(addr, plugin[i++]);
			}
		}
	}
#endif

static void vs1053_hard_reset(void) {
	gtimerInit(&playTimer);

	board_init();
	board_reset();

	#if VS1053_CLK > 12288000
		cmd_writenodreq(SCI_MODE, SCI_MODE_STARTUP);	// Set super-clock before dreq
	#endif

	// Set up registers
	cmd_write(SCI_MODE, SCI_MODE_STARTUP);			// Set mode
	cmd_write(SCI_CLOCKF, SC_CLOCK_MODE);			// Set clocks
	board_init_end();								// Clocks are now set up
	cmd_write(SCI_BASS, 0x0000);					// No treble or bass boost
	cmd_write(SCI_VOL, VOL_MAX);					// Maximum volume

	// Load any firmware
	#if VS1053_FIRMWARE_PATCH
		LoadUserCode();

		// Reset mode just in case
		cmd_write(SCI_MODE, SCI_MODE_STARTUP);
	#endif
}

static void vs1053_soft_reset(void) {
	cmd_write(SCI_MODE, cmd_read(SCI_MODE)|SM_RESET);
	gfxSleepMilliseconds(1);	// Wait for at least 2uS
	waitforready();

	// Reload any firmware
	#if VS1053_FIRMWARE_PATCH
		LoadUserCode();

		// Reset mode just in case
		cmd_write(SCI_MODE, SCI_MODE_STARTUP);
	#endif
}

static uint16_t vs1053_getendbyte(void) {
	cmd_write(SCI_WRAMADDR, WRAMADDR_EXTRAPARAMS+4);
	return cmd_read(SCI_WRAM);
}

static GTimer			playTimer;
static GDataBuffer		*pplay;
static size_t			playlen;
static uint8_t			*pdata;

static void FeedData(void *param) {
	unsigned	len;
	(void)		param;

	// While there is data space
	while (board_dreq()) {

		// Send up to 32 bytes
		len = playlen;
		if (len > 32) len = 32;
		data_write(pdata, len);
		pdata += len;
		playlen -= len;

		// Have we finished the buffer
		while (!playlen) {
			gfxSystemLock();
			gaudioPlayReleaseDataBlockI(pplay);

			// Get a new data buffer
			if (!(pplay = gaudioPlayGetDataBlockI())) {
				// We should really only do the play-done when the audio
				// has really finished playing. Unfortunately there seems
				// to be no documented way of determining this.
				gaudioPlayDoneI();
				gfxSystemUnlock();
				gtimerStop(&playTimer);
				return;
			}

			// Set up ready for the new buffer
			playlen = pplay->len;
			pdata = (uint8_t *)(pplay+1);
			gfxSystemUnlock();
		}
	}
}

/*===========================================================================*/
/* External declarations.                                                    */
/*===========================================================================*/

bool_t gaudio_play_lld_init(uint16_t channel, uint32_t frequency, ArrayDataFormat format) {
	uint32_t	brate;
	uint32_t	bps;
	uint8_t		buf[4];
	static const uint8_t hdr1[] = {
			'R', 'I', 'F', 'F',
			0xFF, 0xFF, 0xFF, 0xFF,
			'W', 'A', 'V', 'E',
			'f', 'm', 't', ' ',
			16, 0, 0, 0,
			0x01, 0x00,
	};
	static const uint8_t hdr2[] = {
			'd', 'a', 't', 'a',
			0xFF, 0xFF, 0xFF, 0xFF,
	};

	if (format != ARRAY_DATA_8BITUNSIGNED && format != ARRAY_DATA_16BITSIGNED && format != ARRAY_DATA_UNKNOWN)
		return FALSE;
	if (frequency > VS1053_MAX_SAMPLE_RATE)
		return FALSE;

	// Reset the chip if needed
	if (!vs1053_isinit) {
		vs1053_hard_reset();
		vs1053_isinit = TRUE;
	}

	// Setup
	if (format == ARRAY_DATA_8BITUNSIGNED || format == ARRAY_DATA_16BITSIGNED) {
		bps = (gfxSampleFormatBits(format)+7)/8;
		if (channel == GAUDIO_PLAY_STEREO)
			bps *= 2;
		brate = frequency * bps;

		// Write the RIFF header
		waitforready();
		data_write(hdr1, sizeof(hdr1));
		buf[0] = channel == GAUDIO_PLAY_STEREO ? 2 : 1;		buf[1] = 0;								data_write(buf, 2);
		buf[0] = frequency;	buf[1] = frequency>>8;	buf[2] = frequency>>16; buf[3] = frequency>>24;	data_write(buf, 4);
		buf[0] = brate;		buf[1] = brate>>8;		buf[2] = brate>>16;		buf[3] = brate>>24;		data_write(buf, 4);
		waitforready();						// 32 bytes max before checking
		buf[0] = bps; 										buf[1] = 0;								data_write(buf, 2);
		buf[0] = gfxSampleFormatBits(format);				buf[1] = 0;								data_write(buf, 2);
		data_write(hdr2, sizeof(hdr2));
	}
	return TRUE;
}

bool_t gaudio_play_lld_set_volume(uint8_t vol) {
	uint16_t tmp;

	// Volume is 0xFE -> 0x00. Adjust vol to match
	vol = ~vol;
	if (vol > 0xFE)
		vol = 0xFE;

	tmp  = 0;
	tmp |= (( vol << VOL_LEFT_SHIFT ) & VOL_LEFT_MASK );
	tmp |= (( vol << VOL_RIGHT_SHIFT ) & VOL_RIGHT_MASK );

	cmd_write(SCI_VOL, tmp);

	return TRUE;
}

void gaudio_play_lld_start(void) {

	gfxSystemLock();
	// Get a new data buffer
	if (pplay || !(pplay = gaudioPlayGetDataBlockI())) {
		gfxSystemUnlock();			// Nothing to do
		return;
	}

	// Set up ready for the new buffer
	playlen = pplay->len;
	pdata = (uint8_t *)(pplay+1);
	gfxSystemUnlock();

	// Start the playing by starting the timer and executing FeedData immediately just to get things started
	// We really should set the timer to be equivalent to half the available data but that is just too hard to calculate.
	gtimerStart(&playTimer, FeedData, 0, TRUE, VS1053_POLL_RATE);
	FeedData(0);
}

void gaudio_play_lld_stop(void) {
	uint8_t		ch;
	unsigned	i;

	// Stop the timer interrupt
	gtimerStop(&playTimer);

	// We may need to clean up the remaining buffer.
	gfxSystemLock();
	if (pplay) {
		gaudioPlayReleaseDataBlockI(pplay);
		pplay = 0;
		gaudioPlayDoneI();
	}
	gfxSystemUnlock();

	// Set CANCEL
	cmd_write(SCI_MODE, cmd_read(SCI_MODE)|SM_CANCEL);

	// Write up to 2048 bytes of data
	ch = 0;
	for(i = 0; i < 2048; i++) {
		if (!(i & 0x1F)) {
			waitforready();
			if (!(cmd_read(SCI_MODE) & SM_CANCEL))
				break;
		}
		data_write(&ch, 1);
	}

	// Make sure the cancel worked
	waitforready();
	if ((cmd_read(SCI_MODE) & SM_CANCEL))
		vs1053_soft_reset();

	// Send the terminating data
	ch = vs1053_getendbyte();
	for(i = 0; i < 2052; i++) {
		if (!(i & 0x1F))
			waitforready();
		data_write(&ch, 1);
	}
}

#endif /* GFX_USE_GAUDIO && GAUDIO_NEED_PLAY */

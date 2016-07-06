/*
 * This file is subject to the terms of the GFX License. If a copy of
 * the license was not distributed with this file, you can obtain one at:
 *
 *              http://ugfx.org/license.html
 */

#include "gfx.h"

#if GFX_USE_GDISP

#define GDISP_DRIVER_VMT			GDISPVMT_ED060SC4
#include "gdisp_lld_config.h"
#include "../../../src/gdisp/gdisp_driver.h"

#include "board_ED060SC4.h"

/*===========================================================================*/
/* Driver local definitions.                                                 */
/*===========================================================================*/

#ifndef GDISP_SCREEN_HEIGHT
	#define GDISP_SCREEN_HEIGHT		600
#endif
#ifndef GDISP_SCREEN_WIDTH
	#define GDISP_SCREEN_WIDTH		800
#endif

/* Number of pixels per byte */
#ifndef EINK_PPB
	#define EINK_PPB 4
#endif

/* Delay for generating clock pulses.
 * Unit is approximate clock cycles of the CPU (0 to 15).
 * This should be atleast 50 ns.
 */
#ifndef EINK_CLOCKDELAY
	#define EINK_CLOCKDELAY 0
#endif

/* Width of one framebuffer block.
 * Must be divisible by EINK_PPB and evenly divide GDISP_SCREEN_WIDTH. */
#ifndef EINK_BLOCKWIDTH
	#define EINK_BLOCKWIDTH 20
#endif

/* Height of one framebuffer block.
 * Must evenly divide GDISP_SCREEN_WIDTH. */
#ifndef EINK_BLOCKHEIGHT
	#define EINK_BLOCKHEIGHT 20
#endif

/* Number of block buffers to use for framebuffer emulation. */
#ifndef EINK_NUMBUFFERS
	#define EINK_NUMBUFFERS 40
#endif

/* Do a "blinking" clear, i.e. clear to opposite polarity first.
 * This reduces the image persistence. */
#ifndef EINK_BLINKCLEAR
	#define EINK_BLINKCLEAR TRUE
#endif

/* Number of passes to use when clearing the display */
#ifndef EINK_CLEARCOUNT
	#define EINK_CLEARCOUNT 10
#endif

/* Number of passes to use when writing to the display */
#ifndef EINK_WRITECOUNT
	#define EINK_WRITECOUNT 4
#endif

/*===========================================================================*/
/* Driver local functions.                                                   */
/*===========================================================================*/

#define PRIV(g)		((drvPriv *)g->priv)

/* Delay between signal changes, to give time for IO pins to change state. */
static GFXINLINE void clockdelay(void)
{
	#if EINK_CLOCKDELAY & 1
	asm("nop");
	#endif
	#if EINK_CLOCKDELAY & 2
	asm("nop");
	asm("nop");
	#endif
	#if EINK_CLOCKDELAY & 4
	asm("nop");
	asm("nop");
	asm("nop");
	asm("nop");
	#endif
	#if EINK_CLOCKDELAY & 8
	asm("nop");
	asm("nop");
	asm("nop");
	asm("nop");
	asm("nop");
	asm("nop");
	asm("nop");
	asm("nop");
	#endif
}

/* Fast vertical clock pulse for gate driver, used during initializations */
static void vclock_quick(GDisplay *g)
{
	setpin_ckv(g, TRUE);
	eink_delay(1);
	setpin_ckv(g, FALSE);
	eink_delay(4);
}

/* Horizontal clock pulse for clocking data into source driver */
static void hclock(GDisplay *g)
{
	clockdelay();
	setpin_cl(g, TRUE);
	clockdelay();
	setpin_cl(g, FALSE);
}

/* Start a new vertical gate driver scan from top.
 * Note: Does not clear any previous bits in the shift register,
 *       so you should always scan through the whole display before
 *       starting a new scan.
 */
static void vscan_start(GDisplay *g)
{
	setpin_gmode(g, TRUE);
	vclock_quick(g);
	setpin_spv(g, FALSE);
	vclock_quick(g);
	setpin_spv(g, TRUE);
	vclock_quick(g);
}

/* Waveform for strobing a row of data onto the display.
 * Attempts to minimize the leaking of color to other rows by having
 * a long idle period after a medium-length strobe period.
 */
static void vscan_write(GDisplay *g)
{
	setpin_ckv(g, TRUE);
	setpin_oe(g, TRUE);
	eink_delay(5);
	setpin_oe(g, FALSE);
	setpin_ckv(g, FALSE);
	eink_delay(200);
}

/* Waveform used when clearing the display. Strobes a row of data to the
 * screen, but does not mind some of it leaking to other rows.
 */
static void vscan_bulkwrite(GDisplay *g)
{
	setpin_ckv(g, TRUE);
	eink_delay(20);
	setpin_ckv(g, FALSE);
	eink_delay(200);
}

/* Waveform for skipping a vertical row without writing anything.
 * Attempts to minimize the amount of change in any row.
 */
static void vscan_skip(GDisplay *g)
{
	setpin_ckv(g, TRUE);
	eink_delay(1);
	setpin_ckv(g, FALSE);
	eink_delay(100);
}

/* Stop the vertical scan. The significance of this escapes me, but it seems
 * necessary or the next vertical scan may be corrupted.
 */
static void vscan_stop(GDisplay *g)
{
	setpin_gmode(g, FALSE);
	vclock_quick(g);
	vclock_quick(g);
	vclock_quick(g);
	vclock_quick(g);
	vclock_quick(g);
}

/* Start updating the source driver data (from left to right). */
static void hscan_start(GDisplay *g)
{
	/* Disable latching and output enable while we are modifying the row. */
	setpin_le(g, FALSE);
	setpin_oe(g, FALSE);
	
	/* The start pulse should remain low for the duration of the row. */
	setpin_sph(g, FALSE);
}

/* Write data to the horizontal row. */
static void hscan_write(GDisplay *g, const uint8_t *data, int count)
{
	while (count--)
	{
		/* Set the next byte on the data pins */
		setpins_data(g, *data++);
		
		/* Give a clock pulse to the shift register */
		hclock(g);
	}
}

/* Finish and transfer the row to the source drivers.
 * Does not set the output enable, so the drivers are not yet active. */
static void hscan_stop(GDisplay *g)
{
	/* End the scan */
	setpin_sph(g, TRUE);
	hclock(g);
	
	/* Latch the new data */
	setpin_le(g, TRUE);
	clockdelay();
	setpin_le(g, FALSE);
}

/* Turn on the power to the E-Ink panel, observing proper power sequencing. */
static void power_on(GDisplay *g)
{
	unsigned i;
	
	/* First the digital power supply and signal levels. */
	setpower_vdd(g, TRUE);
	setpin_le(g, FALSE);
	setpin_oe(g, FALSE);
	setpin_cl(g, FALSE);
	setpin_sph(g, TRUE);
	setpins_data(g, 0);
	setpin_ckv(g, FALSE);
	setpin_gmode(g, FALSE);
	setpin_spv(g, TRUE);
	
	/* Min. 100 microsecond delay after digital supply */
	gfxSleepMicroseconds(100);
	
	/* Then negative voltages and min. 1000 microsecond delay. */
	setpower_vneg(g, TRUE);
	gfxSleepMicroseconds(1000);
	
	/* Finally the positive voltages. */
	setpower_vpos(g, TRUE);
	
	/* Clear the vscan shift register */
	vscan_start(g);
	for (i = 0; i < GDISP_SCREEN_HEIGHT; i++)
		vclock_quick(g);
	vscan_stop(g);
}

/* Turn off the power, observing proper power sequencing. */
static void power_off(GDisplay *g)
{
	/* First the high voltages */
	setpower_vpos(g, FALSE);
	setpower_vneg(g, FALSE);
	
	/* Wait for any capacitors to drain */
	gfxSleepMilliseconds(100);
	
	/* Then put all signals and digital supply to ground. */
	setpin_le(g, FALSE);
	setpin_oe(g, FALSE);
	setpin_cl(g, FALSE);
	setpin_sph(g, FALSE);
	setpins_data(g, 0);
	setpin_ckv(g, FALSE);
	setpin_gmode(g, FALSE);
	setpin_spv(g, FALSE);
	setpower_vdd(g, FALSE);
}

/* ====================================
 *      Framebuffer emulation layer
 * ==================================== */
 
#if EINK_PPB == 4
	#define PIXELMASK 3
	#define PIXEL_WHITE 2
	#define PIXEL_BLACK 1
	#define BYTE_WHITE 0xAA
	#define BYTE_BLACK 0x55
#else
	#error Unsupported EINK_PPB value.
#endif

#if GDISP_SCREEN_HEIGHT % EINK_BLOCKHEIGHT != 0
	#error GDISP_SCREEN_HEIGHT must be evenly divisible by EINK_BLOCKHEIGHT
#endif

#if GDISP_SCREEN_WIDTH % EINK_BLOCKWIDTH != 0
	#error GDISP_SCREEN_WIDTH must be evenly divisible by EINK_BLOCKWIDTH
#endif

#if EINK_BLOCKWIDTH % EINK_PPB != 0
	#error EINK_BLOCKWIDTH must be evenly divisible by EINK_PPB
#endif

#if EINK_NUMBUFFERS > 254
	#error EINK_NUMBUFFERS must be at most 254.
#endif

#define BLOCKS_Y		(GDISP_SCREEN_HEIGHT / EINK_BLOCKHEIGHT)
#define BLOCKS_X		(GDISP_SCREEN_WIDTH / EINK_BLOCKWIDTH)
#define WIDTH_BYTES		(EINK_BLOCKWIDTH / EINK_PPB)

/* Buffers that store the data for a small area of the display. */
typedef struct {
	uint8_t data[EINK_BLOCKHEIGHT][WIDTH_BYTES];
} block_t;

typedef struct drvPriv {
	uint8_t g_next_block; /* Index of the next free block buffer. */
	block_t g_blocks[EINK_NUMBUFFERS];

	/* Map that stores the buffers associated to each area of the display.
	 * Value of 0 means that the block is not allocated.
	 * Other values are the index in g_blocks + 1.
	 */
	uint8_t g_blockmap[BLOCKS_Y][BLOCKS_X];
} drvPriv;

/* Check if the row contains any allocated blocks. */
static bool_t blocks_on_row(GDisplay *g, unsigned by)
{
	unsigned bx;
	for (bx = 0; bx < BLOCKS_X; bx++)
	{
		if (PRIV(g)->g_blockmap[by][bx] != 0)
		{
			return TRUE;
		}
	}
	return FALSE;
}

/* Write out a block row. */
static void write_block_row(GDisplay *g, unsigned by)
{
	unsigned bx, dy, dx;
	for (dy = 0; dy < EINK_BLOCKHEIGHT; dy++)
	{
		hscan_start(g);
		for (bx = 0; bx < BLOCKS_X; bx++)
		{
			if (PRIV(g)->g_blockmap[by][bx] == 0)
			{
				for (dx = 0; dx < WIDTH_BYTES; dx++)
				{
					const uint8_t dummy = 0;
					hscan_write(g, &dummy, 1);
				}
			}
			else
			{
				block_t *block = &PRIV(g)->g_blocks[PRIV(g)->g_blockmap[by][bx] - 1];
				hscan_write(g, &block->data[dy][0], WIDTH_BYTES);
			}
		}
		hscan_stop(g);
		
		vscan_write(g);
	}
}

/* Clear the block map, i.e. deallocate all blocks */
static void clear_block_map(GDisplay *g)
{
	unsigned bx, by;
	for (by = 0; by < BLOCKS_Y; by++)
	{
		for (bx = 0; bx < BLOCKS_X; bx++)
		{
			PRIV(g)->g_blockmap[by][bx] = 0;
		}
	}
	
	PRIV(g)->g_next_block = 0;
}

/* Initialize a newly allocated block. */
static void zero_block(block_t *block)
{
	unsigned dx, dy;
	for (dy = 0; dy < EINK_BLOCKHEIGHT; dy++)
	{
		for (dx = 0; dx < WIDTH_BYTES; dx++)
		{
			block->data[dy][dx] = 0;
		}
	}
}

/* Allocate a buffer
 * Automatically flushes if all buffers are full. */
static block_t *alloc_buffer(GDisplay *g, unsigned bx, unsigned by)
{
	block_t *result;
	drvPriv *priv;

	priv = PRIV(g);
	if (priv->g_blockmap[by][bx] == 0)
	{
		if (priv->g_next_block >= EINK_NUMBUFFERS)
			gdisp_lld_flush(g);
		
		result = &priv->g_blocks[priv->g_next_block];
		priv->g_blockmap[by][bx] = priv->g_next_block + 1;
		priv->g_next_block++;
		zero_block(result);
		return result;
	}
	else
	{
		result = &priv->g_blocks[priv->g_blockmap[by][bx] - 1];
		return result;
	}
}

/*===========================================================================*/
/* Driver exported functions.                                                */
/*===========================================================================*/

LLDSPEC bool_t gdisp_lld_init(GDisplay *g) {
	g->priv = gfxAlloc(sizeof(drvPriv));

	init_board(g);
	
	/* Make sure that all the pins are in "off" state.
	 * Having any pin high could cause voltage leaking to the
	 * display, which in turn causes the image to leak slowly away.
	 */
	power_off(g);
	
	clear_block_map(g);
	
	/* Initialise the GDISP structure */
	g->g.Width = GDISP_SCREEN_WIDTH;
	g->g.Height = GDISP_SCREEN_HEIGHT;
	g->g.Orientation = GDISP_ROTATE_0;
	g->g.Powermode = powerOn;
	g->g.Backlight = 100;
	g->g.Contrast = 100;
	return TRUE;
}

#if GDISP_HARDWARE_FLUSH
	LLDSPEC void gdisp_lld_flush(GDisplay *g) {
		unsigned by, dy, i;

		for (i = 0; i < EINK_WRITECOUNT; i++) {
			vscan_start(g);

			for (by = 0; by < BLOCKS_Y; by++) {
				if (!blocks_on_row(g, by)) {
					/* Skip the whole row of blocks. */
					for (dy = 0; dy < EINK_BLOCKHEIGHT; dy++)
						vscan_skip(g);
				} else {
					/* Write out the blocks. */
					write_block_row(g, by);
				}
			}

			vscan_stop(g);
		}

		clear_block_map(g);
	}
#endif

#if GDISP_HARDWARE_DRAWPIXEL
	void gdisp_lld_draw_pixel(GDisplay *g) {
		block_t *block;
		uint8_t byte;
		unsigned bx, by, dx, dy;
		uint8_t bitpos;

		switch(g->g.Orientation) {
		default:
		case GDISP_ROTATE_0:
			bx = g->p.x / EINK_BLOCKWIDTH;
			dx = g->p.x % EINK_BLOCKWIDTH;
			by = g->p.y / EINK_BLOCKHEIGHT;
			dy = g->p.y % EINK_BLOCKHEIGHT;
			break;
		case GDISP_ROTATE_90:
			bx = g->p.y / EINK_BLOCKWIDTH;
			dx = g->p.y % EINK_BLOCKWIDTH;
			by = (GDISP_SCREEN_HEIGHT-1 - g->p.x) / EINK_BLOCKHEIGHT;
			dy = (GDISP_SCREEN_HEIGHT-1 - g->p.x) % EINK_BLOCKHEIGHT;
			break;
		case GDISP_ROTATE_180:
			bx = (GDISP_SCREEN_WIDTH-1 - g->p.x) / EINK_BLOCKWIDTH;
			dx = (GDISP_SCREEN_WIDTH-1 - g->p.x) % EINK_BLOCKWIDTH;
			by = (GDISP_SCREEN_HEIGHT-1 - g->p.y) / EINK_BLOCKHEIGHT;
			dy = (GDISP_SCREEN_HEIGHT-1 - g->p.y) % EINK_BLOCKHEIGHT;
			break;
		case GDISP_ROTATE_270:
			bx = (GDISP_SCREEN_WIDTH-1 - g->p.y) / EINK_BLOCKWIDTH;
			dx = (GDISP_SCREEN_WIDTH-1 - g->p.y) % EINK_BLOCKWIDTH;
			by = g->p.x / EINK_BLOCKHEIGHT;
			dy = g->p.x % EINK_BLOCKHEIGHT;
			break;
		}

		block = alloc_buffer(g, bx, by);

		bitpos = (6 - 2 * (dx % EINK_PPB));
		byte = block->data[dy][dx / EINK_PPB];
		byte &= ~(PIXELMASK << bitpos);
		if (gdispColor2Native(g->p.color) != Black)
			byte |= PIXEL_WHITE << bitpos;
		else
			byte |= PIXEL_BLACK << bitpos;
		block->data[dy][dx / EINK_PPB] = byte;
	}
#endif
	
#if GDISP_NEED_CONTROL && GDISP_HARDWARE_CONTROL
	LLDSPEC void gdisp_lld_control(GDisplay *g) {
		switch(g->p.x) {
		case GDISP_CONTROL_POWER:
			if (g->g.Powermode == (powermode_t)g->p.ptr)
				return;
			switch((powermode_t)g->p.ptr) {
			case powerOff:
			case powerSleep:
			case powerDeepSleep:
				gdisp_lld_flush(g);
				power_off(g);
				break;
			case powerOn:
				power_on(g);
				break;
			default:
				return;
			}
			g->g.Powermode = (powermode_t)g->p.ptr;
			return;

		case GDISP_CONTROL_ORIENTATION:
			if (g->g.Orientation == (orientation_t)g->p.ptr)
				return;
			switch((orientation_t)g->p.ptr) {
			case GDISP_ROTATE_0:
			case GDISP_ROTATE_180:
				g->g.Height = GDISP_SCREEN_HEIGHT;
				g->g.Width = GDISP_SCREEN_WIDTH;
				break;
			case GDISP_ROTATE_90:
			case GDISP_ROTATE_270:
				g->g.Height = GDISP_SCREEN_WIDTH;
				g->g.Width = GDISP_SCREEN_HEIGHT;
				break;
			default:
				return;
			}
			g->g.Orientation = (orientation_t)g->p.ptr;
			return;
			
		default:
			return;
		}
	}
#endif

/* ===============================
 *       Accelerated routines
 * =============================== */

#if GDISP_HARDWARE_CLEARS
	static void subclear(GDisplay *g, color_t color) {
		unsigned x, y;
		uint8_t byte;

		hscan_start(g);
		byte = color ? BYTE_WHITE : BYTE_BLACK;
		for (x = 0; x < GDISP_SCREEN_WIDTH; x++)
		{
			hscan_write(g, &byte, 1);
		}
		hscan_stop(g);

		setpin_oe(g, TRUE);
		vscan_start(g);
		for (y = 0; y < GDISP_SCREEN_HEIGHT; y++)
			vscan_bulkwrite(g);
		vscan_stop(g);
		setpin_oe(g, FALSE);
	}
	
	void gdisp_lld_clear(GDisplay *g) {
		unsigned i;

		clear_block_map(g);

		if (EINK_BLINKCLEAR) {
			subclear(g, !g->p.color);
			gfxSleepMilliseconds(50);
		}

		for (i = 0; i < EINK_CLEARCOUNT; i++) {
			subclear(g, g->p.color);
			gfxSleepMilliseconds(10);
		}
	}
#endif

#endif	// GFX_USE_GDISP

/*
 * This file is subject to the terms of the GFX License. If a copy of
 * the license was not distributed with this file, you can obtain one at:
 *
 *              http://ugfx.org/license.html
 */

// Set this to your frame buffer pixel format. Note Linux frame buffer only supports RGB modes (no BGR modes).
#ifndef GDISP_LLD_PIXELFORMAT
	#define GDISP_LLD_PIXELFORMAT		GDISP_PIXELFORMAT_RGB888
#endif

#ifdef GDISP_DRIVER_VMT

	#define FBDEV_PATH1		"/dev/fb0"
	#define FBDEV_PATH2		"/dev/fb/0"			// Optional - comment this out to only try the one device
	#define USE_SET_MODE						// Optional - comment this out to not to try to set the color mode we want
	//#define VTDEV_PATH	"/dev/tty0"			// Optional - if defined use this tty to switch from text to graphics mode

	#define _GNU_SOURCE 1
	#include <fcntl.h>
	#include <limits.h>
	#include <linux/fb.h>
	#include <linux/kd.h>
	#include <linux/vt.h>
	#include <stdarg.h>
	#include <stdio.h>
	#include <stdlib.h>
	#include <string.h>
	#include <sys/ioctl.h>
	#include <sys/mman.h>
	#include <sys/stat.h>
	#include <sys/time.h>
	#include <sys/types.h>
	#include <unistd.h>

	#if VTDEV_PATH
		static void board_revert2textmode(void) {
			int tty;

			// Go back to text mode
			if ((tty = open(VTDEV_PATH, O_RDWR)) >= 0) {
				ioctl(tty, KDSETMODE, KD_TEXT);
				close(tty);
			}
		}
		static void board_switch2graphicsmode(void) {
			int tty;

			// Open the tty
			if ((tty = open (VTDEV_PATH, O_RDWR)) < 0) {
				fprintf(stderr, "GDISP Framebuffer: Cannot open tty %s\n", VTDEV_PATH);
				exit(-1);
			}
			if (ioctl (tty, KDSETMODE, KD_GRAPHICS) == -1) {
				fprintf(stderr, "GDISP Framebuffer: Cannot set to graphics mode\n");
				exit(-1);
			}
			close(tty);

			// Make sure we clean up properly
			atexit(board_revert2textmode)
		}
	#endif

	static void board_init(GDisplay *g, fbInfo *fbi) {
		int							fb;
		char *						env;
		size_t						fblen;
		struct fb_fix_screeninfo	fb_fix;
		struct fb_var_screeninfo	fb_var;

		// Open the frame buffer device
		if((env = getenv("FRAMEBUFFER")) != 0)
			fb = open(env, O_RDWR);
		else {
			fb = open(FBDEV_PATH1, O_RDWR);
			#ifdef FBDEV_PATH2
				if (fb < 0) fb = open(FBDEV_PATH2, O_RDWR);
			#endif
		}
		if(fb < 0) {
			fprintf(stderr, "GDISP Framebuffer: Error opening the framebuffer device\n");
			exit(-1);
		}

		// Get screen info
		if (ioctl(fb, FBIOGET_FSCREENINFO, &fb_fix) == -1 || ioctl(fb, FBIOGET_VSCREENINFO, &fb_var) == -1) {
			fprintf(stderr, "GDISP Framebuffer: Error getting screen info\n");
			exit(-1);
		}

		#ifdef USE_SET_MODE
			fb_var.reserved[0] = 0;
			fb_var.reserved[1] = 0;
			fb_var.reserved[2] = 0;
			fb_var.xoffset = 0;
			fb_var.yoffset = 0;
			fb_var.bits_per_pixel = LLDCOLOR_BITS;
			fb_var.activate = FB_ACTIVATE_NOW;
			if (ioctl(fb, FBIOPUT_VSCREENINFO, &fb_var) == -1 || ioctl (fb, FBIOGET_VSCREENINFO, &fb_var) == -1) {
				fprintf(stderr, "GDISP Framebuffer: Failed to set video mode\n");
				exit(-1);
			}
		#endif

		// Check things are as they should be
		if (fb_fix.type != FB_TYPE_PACKED_PIXELS) {
			fprintf(stderr, "GDISP Framebuffer: The display is not in a single plane graphics mode\n");
			exit(-1);
		}
		if (fb_fix.visual != FB_VISUAL_TRUECOLOR) {
			fprintf(stderr, "GDISP Framebuffer: The display is not in TRUECOLOR mode\n");
			exit(-1);
		}
		if (fb_var.bits_per_pixel != LLDCOLOR_TYPE_BITS) {
			fprintf(stderr, "GDISP Framebuffer: The display is %u not %u bits per pixel\n", fb_var.bits_per_pixel, LLDCOLOR_TYPE_BITS);
			exit(-1);
		}
		if (fb_var.red.length != LLDCOLOR_BITS_R || fb_var.green.length != LLDCOLOR_BITS_G || fb_var.blue.length != LLDCOLOR_BITS_B) {
			fprintf(stderr, "GDISP Framebuffer: The display pixel format is not %d%d%d\n", LLDCOLOR_BITS_R, LLDCOLOR_BITS_G, LLDCOLOR_BITS_B);
			exit(-1);
		}
		if (fb_var.red.offset != LLDCOLOR_SHIFT_R || fb_var.green.offset != LLDCOLOR_SHIFT_G || fb_var.blue.offset != LLDCOLOR_SHIFT_B) {
			#if LLDCOLOR_SHIFT_B == 0
				fprintf(stderr, "GDISP Framebuffer: The display pixel format is not RGB\n");
			#else
				fprintf(stderr, "GDISP Framebuffer: The display pixel format is not BGR\n");
			#endif
			exit(-1);
		}

		// Ensure we are at the origin of the virtual display area
		if (fb_var.xoffset || fb_var.yoffset) {
			fb_var.xoffset = 0;
			fb_var.yoffset = 0;
			ioctl(fb, FBIOPAN_DISPLAY, &fb_var);
		}

		// Switch to graphics mode (if required)
		#ifdef VTDEV_PATH
			board_switch2graphicsmode();
		#endif

		// Calculate the frame buffer length
		fblen = fb_var.yres * fb_fix.line_length;

		// Different systems need mapping in slightly different ways - Yuck!
		#ifdef ARCH_LINUX_SPARC
			#define CG3_MMAP_OFFSET 0x4000000
			#define CG6_RAM    		0x70016000
			#define TCX_RAM8BIT		0x00000000
			#define TCX_RAM24BIT	0x01000000
			switch (fb_fix.accel) {
			case FB_ACCEL_SUN_CGTHREE:
				fbi->pixels = mmap(0, fblen, PROT_READ|PROT_WRITE, MAP_SHARED, fb, CG3_MMAP_OFFSET);
				break;
			case FB_ACCEL_SUN_CGSIX:
				fbi->pixels = mmap(0, fblen, PROT_READ|PROT_WRITE, MAP_SHARED, fb, CG6_RAM);
				break;
			case FB_ACCEL_SUN_TCX:
				fbi->pixels = mmap(0, fblen, PROT_READ|PROT_WRITE, MAP_SHARED, fb, TCX_RAM24BIT);
				break;
			default:
				fprintf(stderr, "GDISP Framebuffer: Don't know how to mmap with accel %d\n", fb_fix.accel);
				exit(-1);
			}
		#elif defined(BLACKFIN)
			fbi->pixels = mmap(0, fblen, PROT_READ|PROT_WRITE, MAP_PRIVATE|MAP_FILE, fb, 0);
		#elif defined(__uClinux__)
			fbi->pixels = mmap(0, fblen, PROT_READ|PROT_WRITE, 0, fb, 0);
		#else
			fbi->pixels = mmap(0, fblen, PROT_READ|PROT_WRITE, MAP_SHARED, fb, 0);
		#endif
		if(!fbi->pixels || fbi->pixels == (void *)-1) {
			fprintf(stderr, "GDISP Framebuffer: mmap of display buffer failed\n");
			exit(-1);
		}

		// If this program gets children they should not inherit this file descriptor
		fcntl(fb, F_SETFD, FD_CLOEXEC);

		// We are finished with the file descriptor
		close(fb);

		// Set the rest of the details of the frame buffer
		g->g.Width = fb_var.xres;
		g->g.Height = fb_var.yres;
		g->g.Backlight = 100;
		g->g.Contrast = 50;
		fbi->linelen = fb_fix.line_length;
	}

	#if GDISP_HARDWARE_FLUSH
		static void board_flush(GDisplay *g) {
			(void) g;
		}
	#endif

	#if GDISP_NEED_CONTROL
		static void board_backlight(GDisplay *g, uint8_t percent) {
			(void) g;
			(void) percent;
		}

		static void board_contrast(GDisplay *g, uint8_t percent) {
			(void) g;
			(void) percent;
		}

		static void board_power(GDisplay *g, powermode_t pwr) {
			(void) g;
			(void) pwr;
		}
	#endif

#endif /* GDISP_LLD_BOARD_IMPLEMENTATION */

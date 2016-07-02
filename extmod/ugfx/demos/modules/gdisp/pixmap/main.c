/*
 * Copyright (c) 2012, 2013, Joel Bodenmann aka Tectu <joel@unormal.org>
 * Copyright (c) 2012, 2013, Andrew Hannam aka inmarket
 *
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *    * Redistributions of source code must retain the above copyright
 *      notice, this list of conditions and the following disclaimer.
 *    * Redistributions in binary form must reproduce the above copyright
 *      notice, this list of conditions and the following disclaimer in the
 *      documentation and/or other materials provided with the distribution.
 *    * Neither the name of the <organization> nor the
 *      names of its contributors may be used to endorse or promote products
 *      derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL <COPYRIGHT HOLDER> BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "gfx.h"

#define PIXMAP_WIDTH	40
#define PIXMAP_HEIGHT	10

static GDisplay* pixmap;
static pixel_t* surface;

int main(void) {
	coord_t		width, height;
	coord_t		i, j;

    // Initialize and clear the display
    gfxInit();

    // Get the screen size
    width = gdispGetWidth();
    height = gdispGetHeight();

    // Create a pixmap and get a pointer to the bits
    pixmap = gdispPixmapCreate(PIXMAP_WIDTH, PIXMAP_HEIGHT);
    surface = gdispPixmapGetBits(pixmap);

    // A pixmap can be treated either as a virtual display or as a memory framebuffer surface.
    // We demonstrate writing to it using both methods.

    // First demo drawing onto the surface directly
    for(j = 0; j < PIXMAP_HEIGHT; j++)
    	for(i = 0; i < PIXMAP_WIDTH; i++)
    		surface[j*PIXMAP_WIDTH + i] = RGB2COLOR(0, 255-i*(256/PIXMAP_WIDTH), j*(256/PIXMAP_HEIGHT));

    // Secondly, show drawing a line on it like a virtual display
    gdispGDrawLine(pixmap, 0, 0, gdispGGetWidth(pixmap)-1, gdispGGetHeight(pixmap)-1, White);
    
    i = j = 0;
    while(TRUE) {
    	// Clear the old position
    	gdispFillArea(i, j, PIXMAP_WIDTH, PIXMAP_HEIGHT, Black);

    	// Change the position
    	i += PIXMAP_WIDTH/2;
    	if (i >= width - PIXMAP_WIDTH/2) {
    		i %= width - PIXMAP_WIDTH/2;
    		j = (j + PIXMAP_HEIGHT/2) % (height - PIXMAP_HEIGHT/2);
    	}

    	// Blit the pixmap to the real display at the new position
    	gdispBlitArea(i, j, PIXMAP_WIDTH, PIXMAP_HEIGHT, surface);

    	// Wait
    	gfxSleepMilliseconds(100);
    }

    // Clean up
    gdispPixmapDelete(pixmap);
}


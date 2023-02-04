/*
 * This file is part of the Micro Python project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2018 Scott Shawcroft for Adafruit Industries
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

#include "shared-bindings/displayio/OnDiskGif.h"
#include "shared-bindings/displayio/ColorConverter.h"
#include "shared-bindings/displayio/Palette.h"
#include "shared-bindings/displayio/Bitmap.h"
#include "shared-module/displayio/ColorConverter.h"
#include "shared-module/displayio/Palette.h"

#include <string.h>

#include "py/mperrno.h"
#include "py/runtime.h"

static int32_t GIFReadFile(GIFFILE *pFile, uint8_t *pBuf, int32_t iLen) {
    // mp_printf(&mp_plat_print, "GifReadFile len %d ", iLen);
    uint32_t iBytesRead;
    iBytesRead = iLen;
    pyb_file_obj_t *f = pFile->fHandle;
    // Note: If you read a file all the way to the last byte, seek() stops working
    if ((pFile->iSize - pFile->iPos) < iLen) {
        iBytesRead = pFile->iSize - pFile->iPos - 1; // <-- ugly work-around
    }
    if (iBytesRead <= 0) {
        return 0;
    }
    UINT bytes_read;
    if (f_read(&f->fp, pBuf, iBytesRead, &bytes_read) != FR_OK) {
        mp_raise_OSError(MP_EIO);
    }
    pFile->iPos = f->fp.fptr;
    // mp_printf(&mp_plat_print, " now at %d\n", pFile->iPos);

    return bytes_read;
} /* GIFReadFile() */

static int32_t GIFSeekFile(GIFFILE *pFile, int32_t iPosition) {
    // mp_printf(&mp_plat_print, "GifSeekFile %d ", iPosition);
    pyb_file_obj_t *f = pFile->fHandle;

    f_lseek(&f->fp, iPosition);
    pFile->iPos = f->fp.fptr;
    // mp_printf(&mp_plat_print, " now at %d\n", pFile->iPos);
    return pFile->iPos;
} /* GIFSeekFile() */

static void GIFDraw(GIFDRAW *pDraw) {
    // Called for every scan line of the image as it decodes
    // The pixels delivered are the 8-bit native GIF output
    // The palette is either RGB565 or the original 24-bit RGB values
    // depending on the pixel type selected with gif.begin()

    displayio_bitmap_t *bitmap = (displayio_bitmap_t *)pDraw->pUser;

    // mp_printf(&mp_plat_print, "GD: y%d iX%d iY%d iW%d iH%d Trans%d hasT%d bk%d pUser %x bmp%x\n",
    // pDraw->y, pDraw->iX, pDraw->iY, pDraw->iWidth, pDraw->iHeight, pDraw->ucTransparent, pDraw->ucHasTransparency, pDraw->ucBackground, pDraw->pUser, bitmap->data);

/*
    int iX, iY; // Corner offset of this frame on the canvas
    int y; // current line being drawn (0 = top line of image)
    int iWidth, iHeight; // size of this frame
    void *pUser; // user supplied pointer
    uint8_t *pPixels; // 8-bit source pixels for this line
    uint16_t *pPalette; // little or big-endian RGB565 palette entries (default)
    uint8_t *pPalette24; // RGB888 palette (optional)
    uint8_t ucTransparent; // transparent color
    uint8_t ucHasTransparency; // flag indicating the transparent color is in use
    uint8_t ucDisposalMethod; // frame disposal method
    uint8_t ucBackground; // background color
    uint8_t ucIsGlobalPalette; // Flag to indicate that a global palette, rather than a local palette is being used
*/

    // For all other lines, just push the pixels to the display
    // (uint8_t *)pDraw->pPixels, pDraw->iWidth*2);



    int32_t row_start = pDraw->y * bitmap->stride;
    uint32_t *row = bitmap->data + row_start;
    uint8_t *d = (uint8_t *)row;
    // mp_printf(&mp_plat_print, "rs %d strd %d:\n", row_start, bitmap->stride);

    uint8_t *p = (uint8_t *)pDraw->pPixels;
    for (int w = 0; w < pDraw->iWidth * 2; w++) {
        // mp_printf(&mp_plat_print, "%x:", *p);
        *d++ = *p;
        p++;
    }
    // mp_printf(&mp_plat_print, "\n");
}

void common_hal_displayio_ondiskgif_construct(displayio_ondiskgif_t *self, pyb_file_obj_t *file) {
    // mp_printf(&mp_plat_print, "Begin OnDiskGif\n");
    self->file = file;

    GIF_begin(&self->gif, GIF_PALETTE_RGB565_LE);

    self->gif.iError = GIF_SUCCESS;
    self->gif.pfnRead = GIFReadFile;
    self->gif.pfnSeek = GIFSeekFile;
    self->gif.pfnDraw = GIFDraw;
    self->gif.pfnClose = NULL;
    self->gif.pfnOpen = NULL;
    self->gif.GIFFile.fHandle = self->file;

    f_rewind(&self->file->fp);
    self->gif.GIFFile.iSize = (int32_t)f_size(&self->file->fp);

    int result = GIF_init(&self->gif);
    if (result != 1) {
        mp_arg_error_invalid(MP_QSTR_file);
    }

    self->frame = m_malloc(self->gif.iCanvasWidth * self->gif.iCanvasHeight * sizeof(uint16_t), false); // MUST FREE LATER?
    self->gif.pFrameBuffer = (uint8_t *)self->frame;
    self->gif.ucDrawType = GIF_DRAW_COOKED;

    displayio_bitmap_t *bitmap = m_new_obj(displayio_bitmap_t);
    bitmap->base.type = &displayio_bitmap_type;
    common_hal_displayio_bitmap_construct(bitmap, self->gif.iCanvasWidth, self->gif.iCanvasHeight, 16);
    self->bitmap = bitmap;

    // mp_printf(&mp_plat_print, "GIF_init returned %d %x\n", result, bitmap->data);
}


uint32_t common_hal_displayio_ondiskgif_get_pixel(displayio_ondiskgif_t *self,
    int16_t x, int16_t y) {
    if (x < 0 || x >= self->gif.iCanvasWidth || y < 0 || y >= self->gif.iCanvasHeight) {
        return 0;
    }

    return 0;
}

uint16_t common_hal_displayio_ondiskgif_get_height(displayio_ondiskgif_t *self) {
    GIFINFO info;
    GIF_getInfo(&self->gif, &info);
    mp_printf(&mp_plat_print, "dur %d fc %d max %d min %d\n", info.iDuration, info.iFrameCount, info.iMaxDelay, info.iMinDelay);

    return (uint16_t)self->gif.iCanvasHeight;
}

uint16_t common_hal_displayio_ondiskgif_get_width(displayio_ondiskgif_t *self) {
    return (uint16_t)self->gif.iCanvasWidth;
}

mp_obj_t common_hal_displayio_ondiskgif_get_pixel_shader(displayio_ondiskgif_t *self) {
    return MP_OBJ_FROM_PTR(self->pixel_shader_base);
}

mp_obj_t common_hal_displayio_ondiskgif_get_bitmap(displayio_ondiskgif_t *self) {
    return MP_OBJ_FROM_PTR(self->bitmap);
}

uint8_t common_hal_displayio_ondiskgif_play_frame(displayio_ondiskgif_t *self) {
    int result = GIF_playFrame(&self->gif, 0, self->bitmap);

    if (result >= 0) {
        displayio_area_t dirty_area = {
            .x1 = 0,
            .y1 = 0,
            .x2 = self->bitmap->width,
            .y2 = self->bitmap->height,
        };

        displayio_bitmap_set_dirty_area(self->bitmap, &dirty_area);
    }

    return result;
}

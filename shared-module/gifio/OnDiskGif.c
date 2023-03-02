/*
 * This file is part of the Micro Python project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2023 Mark Komus
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

#include "shared-bindings/gifio/OnDiskGif.h"
#include "shared-bindings/displayio/Bitmap.h"

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

    uint8_t *s;
    uint16_t *d;

    int iWidth = pDraw->iWidth;
    if (iWidth + pDraw->iX > bitmap->width) {
        iWidth = bitmap->width - pDraw->iX;
    }

    if (pDraw->iY + pDraw->y >= bitmap->height || pDraw->iX >= bitmap->width || iWidth < 1) {
        return;
    }

    int32_t row_start = (pDraw->y + pDraw->iY) * bitmap->stride;
    uint32_t *row = bitmap->data + row_start;
    s = pDraw->pPixels;
    d = (uint16_t *)row;

    uint16_t *pPal;
    pPal = (uint16_t *)pDraw->pPalette;

    if (pDraw->ucDisposalMethod == 2) { // restore to background color
        // Not supported currently. Need to reset the area the previous frame occupied
        // to the background color before the previous frame was drawn
        // See: https://github.com/bitbank2/AnimatedGIF/issues/3

        // To workaround clear the gif.bitmap object yourself as required.
    }

    uint8_t c, ucTransparent = pDraw->ucTransparent;
    d += pDraw->iX;
    if (pDraw->ucHasTransparency == 1) {
        for (int x = 0; x < iWidth; x++)
        {
            c = *s++;
            if (c != ucTransparent) {
                *d = pPal[c];
            }
            d++;
        }
    } else {
        for (int x = 0; x < iWidth; x++)
        {
            c = *s++;
            *d++ = pPal[c];
        }
    }
}

void common_hal_gifio_ondiskgif_construct(gifio_ondiskgif_t *self, pyb_file_obj_t *file) {
    // mp_printf(&mp_plat_print, "Begin OnDiskGif\n");
    self->file = file;

    GIF_begin(&self->gif, GIF_PALETTE_RGB565_BE);

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

    displayio_bitmap_t *bitmap = m_new_obj(displayio_bitmap_t);
    bitmap->base.type = &displayio_bitmap_type;
    common_hal_displayio_bitmap_construct(bitmap, self->gif.iCanvasWidth, self->gif.iCanvasHeight, 16);
    self->bitmap = bitmap;

    GIFINFO info;
    GIF_getInfo(&self->gif, &info);
    self->duration = info.iDuration;
    self->frame_count = info.iFrameCount;
    self->min_delay = info.iMinDelay;
    self->max_delay = info.iMaxDelay;

    // mp_printf(&mp_plat_print, "GIF_init returned %d %x\n", result, bitmap->data);
}

uint16_t common_hal_gifio_ondiskgif_get_height(gifio_ondiskgif_t *self) {
    return (uint16_t)self->gif.iCanvasHeight;
}

uint16_t common_hal_gifio_ondiskgif_get_width(gifio_ondiskgif_t *self) {
    return (uint16_t)self->gif.iCanvasWidth;
}

mp_obj_t common_hal_gifio_ondiskgif_get_bitmap(gifio_ondiskgif_t *self) {
    return MP_OBJ_FROM_PTR(self->bitmap);
}

int32_t common_hal_gifio_ondiskgif_get_duration(gifio_ondiskgif_t *self) {
    return self->duration;
}

int32_t common_hal_gifio_ondiskgif_get_frame_count(gifio_ondiskgif_t *self) {
    return self->frame_count;
}

int32_t common_hal_gifio_ondiskgif_get_min_delay(gifio_ondiskgif_t *self) {
    return self->min_delay;
}

int32_t common_hal_gifio_ondiskgif_get_max_delay(gifio_ondiskgif_t *self) {
    return self->max_delay;
}

uint32_t common_hal_gifio_ondiskgif_next_frame(gifio_ondiskgif_t *self, bool setDirty) {
    int nextDelay = 0;
    int result = GIF_playFrame(&self->gif, &nextDelay, self->bitmap);

    if ((result >= 0) && (setDirty)) {
        displayio_area_t dirty_area = {
            .x1 = 0,
            .y1 = 0,
            .x2 = self->bitmap->width,
            .y2 = self->bitmap->height,
        };

        displayio_bitmap_set_dirty_area(self->bitmap, &dirty_area);
    }

    return nextDelay;
}

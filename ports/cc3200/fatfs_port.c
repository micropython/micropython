/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2013-2017 Damien P. George
 * Parts of this file are (C)ChaN, 2014, from FatFs option/syscall.c
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

#include "py/runtime.h"
#include "lib/oofatfs/ff.h"
#include "lib/timeutils/timeutils.h"
#include "mods/pybrtc.h"

#if _FS_REENTRANT
// Create a Synchronization Object
// This function is called in f_mount() function to create a new
// synchronization object, such as semaphore and mutex.
// A return of 0 indicates failure, and then f_mount() fails with FR_INT_ERR.
int ff_cre_syncobj(FATFS *fatfs, _SYNC_t *sobj) {
    vSemaphoreCreateBinary((*sobj));
    return (int)(*sobj != NULL);
}

// Delete a Synchronization Object
// This function is called in f_mount() function to delete a synchronization
// object that created with ff_cre_syncobj function.
// A return of 0 indicates failure, and then f_mount() fails with FR_INT_ERR.
int ff_del_syncobj(_SYNC_t sobj) {
    vSemaphoreDelete(sobj);
    return 1;
}

// Request Grant to Access the Volume
// This function is called on entering file functions to lock the volume.
// When a 0 is returned, the file function fails with FR_TIMEOUT.
int ff_req_grant(_SYNC_t sobj) {
    return (int)(xSemaphoreTake(sobj, _FS_TIMEOUT) == pdTRUE);
}

// Release Grant to Access the Volume
// This function is called on leaving file functions to unlock the volume.
void ff_rel_grant(_SYNC_t sobj) {
    xSemaphoreGive(sobj);
}

#endif

DWORD get_fattime(void) {
    timeutils_struct_time_t tm;
    timeutils_seconds_since_2000_to_struct_time(pyb_rtc_get_seconds(), &tm);

    return ((tm.tm_year - 1980) << 25) | ((tm.tm_mon) << 21)  |
            ((tm.tm_mday) << 16)       | ((tm.tm_hour) << 11) |
            ((tm.tm_min) << 5)         | (tm.tm_sec >> 1);
}

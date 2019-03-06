/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2019 Damien P. George
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

#define FFCONF_DEF 86604

#define FF_FS_READONLY      1
#define FF_FS_MINIMIZE      0
#define FF_USE_STRFUNC      0

#define FF_USE_FIND         0
#define FF_USE_MKFS         0
#define FF_USE_FASTSEEK     0
#define FF_USE_EXPAND       0
#define FF_USE_CHMOD        0
#define FF_USE_LABEL        0
#define FF_USE_FORWARD      0

#define FF_CODE_PAGE        437
#define FF_USE_LFN          1
#define FF_MAX_LFN          255
#define FF_LFN_UNICODE      0
#define FF_LFN_BUF          255
#define FF_SFN_BUF          12
#define FF_STRF_ENCODE      3
#define FF_FS_RPATH         0

#define FF_VOLUMES          1
#define FF_STR_VOLUME_ID    0
#define FF_MULTI_PARTITION  0
#define FF_MIN_SS           512
#define FF_MAX_SS           512
#define FF_USE_TRIM         0
#define FF_FS_NOFSINFO      0

#define FF_FS_TINY          1
#define FF_FS_EXFAT         0
#define FF_FS_NORTC         1
#define FF_NORTC_MON        1
#define FF_NORTC_MDAY       1
#define FF_NORTC_YEAR       2019
#define FF_FS_LOCK          0
#define FF_FS_REENTRANT     0

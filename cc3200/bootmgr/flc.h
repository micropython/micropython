/*
 * This file is part of the Micro Python project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2015 Daniel Campora
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

#ifndef __FLC_H__
#define __FLC_H__

/******************************************************************************

   If building with a C++ compiler, make all of the definitions in this header
   have a C binding.
  
*******************************************************************************/
#ifdef __cplusplus
extern "C"
{
#endif

/******************************************************************************
   Image file names
*******************************************************************************/
#define IMG_BOOT_INFO           "/sys/bootinfo.bin"
#define IMG_FACTORY             "/sys/factimg.bin"
#define IMG_UPDATE1             "/sys/updtimg1.bin"
#define IMG_UPDATE2             "/sys/updtimg2.bin"
#define IMG_PREFIX              "/sys/updtimg"

#define IMG_SRVPACK             "/sys/servicepack.ucf"
#define SRVPACK_SIGN            "/sys/servicepack.sig"

#define CA_FILE                 "/cert/ca.pem"
#define CERT_FILE               "/cert/cert.pem"
#define KEY_FILE                "/cert/private.key"

/******************************************************************************
   Special file sizes
*******************************************************************************/
#define IMG_SIZE                (192 * 1024)    /* 16KB are reserved for the bootloader and at least 48KB for the heap*/
#define SRVPACK_SIZE            (16  * 1024)
#define SIGN_SIZE               (2   * 1024)
#define CA_KEY_SIZE             (4   * 1024)

/******************************************************************************
   Active Image
*******************************************************************************/
#define IMG_ACT_FACTORY         0
#define IMG_ACT_UPDATE1         1
#define IMG_ACT_UPDATE2         2

#define IMG_STATUS_CHECK        0
#define IMG_STATUS_READY        1

/******************************************************************************
   Boot Info structure
*******************************************************************************/
typedef struct _sBootInfo_t
{
  _u8  ActiveImg;
  _u8  Status;
  _u8  PrevImg;
  _u8  : 8;
} sBootInfo_t;


/******************************************************************************
  
   Mark the end of the C bindings section for C++ compilers.
  
*******************************************************************************/
#ifdef __cplusplus
}
#endif

#endif /* __FLC_H__ */

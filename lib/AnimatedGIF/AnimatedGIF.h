// Copyright 2020 BitBank Software, Inc. All Rights Reserved.
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//    http://www.apache.org/licenses/LICENSE-2.0
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//===========================================================================

#ifndef __ANIMATEDGIF__
#define __ANIMATEDGIF__
#if defined( PICO_BUILD ) || defined( __MACH__ ) || defined( __LINUX__ ) || defined( __MCUXPRESSO )
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#define memcpy_P memcpy
#define PROGMEM
#else
#include <Arduino.h>
#endif
//
// GIF Animator
// Written by Larry Bank
// Copyright (c) 2020 BitBank Software, Inc.
// bitbank@pobox.com
//
// Designed to decode images up to 480x320
// using less than 22K of RAM
//

/* GIF Defines and variables */
#define MAX_CHUNK_SIZE 255
#define LZW_BUF_SIZE (6*MAX_CHUNK_SIZE)
#define LZW_HIGHWATER (4*MAX_CHUNK_SIZE)
#ifdef __LINUX__
#define MAX_WIDTH 2048
#else
#define MAX_WIDTH 320
#endif // __LINUX__
#define FILE_BUF_SIZE 4096

#define PIXEL_FIRST 0
#define PIXEL_LAST 4096
#define LINK_UNUSED 5911 // 0x1717 to use memset
#define LINK_END 5912
#define MAX_HASH 5003
#define MAXMAXCODE 4096

enum {
   GIF_PALETTE_RGB565_LE = 0, // little endian (default)
   GIF_PALETTE_RGB565_BE,     // big endian
   GIF_PALETTE_RGB888         // original 24-bpp entries
};
// for compatibility with older code
#define LITTLE_ENDIAN_PIXELS GIF_PALETTE_RGB565_LE
#define BIG_ENDIAN_PIXELS GIF_PALETTE_RGB565_BE
//
// Draw callback pixel type
// RAW = 8-bit palettized pixels requiring transparent pixel handling
// COOKED = 16 or 24-bpp fully rendered pixels ready for display
//
enum {
   GIF_DRAW_RAW = 0,
   GIF_DRAW_COOKED
};

enum {
   GIF_SUCCESS = 0,
   GIF_DECODE_ERROR,
   GIF_TOO_WIDE,
   GIF_INVALID_PARAMETER,
   GIF_UNSUPPORTED_FEATURE,
   GIF_FILE_NOT_OPEN,
   GIF_EARLY_EOF,
   GIF_EMPTY_FRAME,
   GIF_BAD_FILE,
   GIF_ERROR_MEMORY
};

typedef struct gif_file_tag
{
  int32_t iPos; // current file position
  int32_t iSize; // file size
  uint8_t *pData; // memory file pointer
  void * fHandle; // class pointer to File/SdFat or whatever you want
} GIFFILE;

typedef struct gif_info_tag
{
  int32_t iFrameCount; // total frames in file
  int32_t iDuration; // duration of animation in milliseconds
  int32_t iMaxDelay; // maximum frame delay
  int32_t iMinDelay; // minimum frame delay
} GIFINFO;

typedef struct gif_draw_tag
{
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
} GIFDRAW;

// Callback function prototypes
typedef int32_t (GIF_READ_CALLBACK)(GIFFILE *pFile, uint8_t *pBuf, int32_t iLen);
typedef int32_t (GIF_SEEK_CALLBACK)(GIFFILE *pFile, int32_t iPosition);
typedef void (GIF_DRAW_CALLBACK)(GIFDRAW *pDraw);
typedef void * (GIF_OPEN_CALLBACK)(const char *szFilename, int32_t *pFileSize);
typedef void (GIF_CLOSE_CALLBACK)(void *pHandle);
typedef void * (GIF_ALLOC_CALLBACK)(uint32_t iSize);
typedef void (GIF_FREE_CALLBACK)(void *buffer);
//
// our private structure to hold a GIF image decode state
//
typedef struct gif_image_tag
{
    int iWidth, iHeight, iCanvasWidth, iCanvasHeight;
    int iX, iY; // GIF corner offset
    int iBpp;
    int iError; // last error
    int iFrameDelay; // delay in milliseconds for this frame
    int iRepeatCount; // NETSCAPE animation repeat count. 0=forever
    int iXCount, iYCount; // decoding position in image (countdown values)
    int iLZWOff; // current LZW data offset
    int iLZWSize; // current quantity of data in the LZW buffer
    int iCommentPos; // file offset of start of comment data
    short sCommentLen; // length of comment
    GIF_READ_CALLBACK *pfnRead;
    GIF_SEEK_CALLBACK *pfnSeek;
    GIF_DRAW_CALLBACK *pfnDraw;
    GIF_OPEN_CALLBACK *pfnOpen;
    GIF_CLOSE_CALLBACK *pfnClose;
    GIFFILE GIFFile;
    void *pUser;
    unsigned char *pFrameBuffer;
    unsigned char *pPixels, *pOldPixels;
    unsigned char ucLineBuf[MAX_WIDTH]; // current line
    unsigned char ucFileBuf[FILE_BUF_SIZE]; // holds temp data and pixel stack
    unsigned short pPalette[384]; // can hold RGB565 or RGB888 - set in begin()
    unsigned short pLocalPalette[384]; // color palettes for GIF images
    unsigned char ucLZW[LZW_BUF_SIZE]; // holds 6 chunks (6x255) of GIF LZW data packed together
    unsigned short usGIFTable[4096];
    unsigned char ucGIFPixels[8192];
    unsigned char bEndOfFrame;
    unsigned char ucGIFBits, ucBackground, ucTransparent, ucCodeStart, ucMap, bUseLocalPalette;
    unsigned char ucPaletteType; // RGB565 or RGB888
    unsigned char ucDrawType; // RAW or COOKED
} GIFIMAGE;

#ifdef __cplusplus
//
// The GIF class wraps portable C code which does the actual work
//
class AnimatedGIF
{
  public:
    int open(uint8_t *pData, int iDataSize, GIF_DRAW_CALLBACK *pfnDraw);
    int openFLASH(uint8_t *pData, int iDataSize, GIF_DRAW_CALLBACK *pfnDraw);
    int open(const char *szFilename, GIF_OPEN_CALLBACK *pfnOpen, GIF_CLOSE_CALLBACK *pfnClose, GIF_READ_CALLBACK *pfnRead, GIF_SEEK_CALLBACK *pfnSeek, GIF_DRAW_CALLBACK *pfnDraw);
    void close();
    void reset();
    void begin(unsigned char ucPaletteType = GIF_PALETTE_RGB565_LE);
    void begin(int iEndian, unsigned char ucPaletteType) { begin(ucPaletteType); };
    int playFrame(bool bSync, int *delayMilliseconds, void *pUser = NULL);
    int getCanvasWidth();
    int allocFrameBuf(GIF_ALLOC_CALLBACK *pfnAlloc);
    int setDrawType(int iType);
    int freeFrameBuf(GIF_FREE_CALLBACK *pfnFree);
    uint8_t *getFrameBuf();
    int getCanvasHeight();
    int getLoopCount();
    int getInfo(GIFINFO *pInfo);
    int getLastError();
    int getComment(char *destBuffer);

  private:
    GIFIMAGE _gif;
};
#else
// C interface
    int GIF_openRAM(GIFIMAGE *pGIF, uint8_t *pData, int iDataSize, GIF_DRAW_CALLBACK *pfnDraw);
    int GIF_openFile(GIFIMAGE *pGIF, const char *szFilename, GIF_DRAW_CALLBACK *pfnDraw);
    void GIF_close(GIFIMAGE *pGIF);
    void GIF_begin(GIFIMAGE *pGIF, unsigned char ucPaletteType);
    void GIF_reset(GIFIMAGE *pGIF);
    int GIF_playFrame(GIFIMAGE *pGIF, int *delayMilliseconds, void *pUser);
    int GIF_getCanvasWidth(GIFIMAGE *pGIF);
    int GIF_getCanvasHeight(GIFIMAGE *pGIF);
    int GIF_getComment(GIFIMAGE *pGIF, char *destBuffer);
    int GIF_getInfo(GIFIMAGE *pGIF, GIFINFO *pInfo);
    int GIF_getLastError(GIFIMAGE *pGIF);
    int GIF_getLoopCount(GIFIMAGE *pGIF);
#endif // __cplusplus

// Due to unaligned memory causing an exception, we have to do these macros the slow way
#define INTELSHORT(p) ((*p) + (*(p+1)<<8))
#define INTELLONG(p) ((*p) + (*(p+1)<<8) + (*(p+2)<<16) + (*(p+3)<<24))
#define MOTOSHORT(p) (((*(p))<<8) + (*(p+1)))
#define MOTOLONG(p) (((*p)<<24) + ((*(p+1))<<16) + ((*(p+2))<<8) + (*(p+3)))

// Must be a 32-bit target processor
#define REGISTER_WIDTH 32

#endif // __ANIMATEDGIF__

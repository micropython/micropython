/*
 * WMXZ Teensy uSDFS library
 * Copyright (c) 2016 Walter Zimmer.
 *
 * Permission is hereby granted, free of charge, to any person obtaining
 * a copy of this software and associated documentation files (the
 * "Software"), to deal in the Software without restriction, including
 * without limitation the rights to use, copy, modify, merge, publish,
 * distribute, sublicense, and/or sell copies of the Software, and to
 * permit persons to whom the Software is furnished to do so, subject to
 * the following conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS
 * BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN
 * ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */
//sdhc.h

// following code is modified  by Walter Zimmer from 
// version provided by
// Petr Gargulak (NXP Employee) 
//https://community.nxp.com/servlet/JiveServlet/download/339474-1-263510/SDHC_K60_Baremetal.ZIP
//see also
//https://community.nxp.com/thread/99202

#ifndef _SDHC_H
#define _SDHC_H
/******************************************************************************
* Includes
******************************************************************************/

#include "kinetis.h"

// type definitions for 'original' sdhc.c 
typedef unsigned int   LWord;
typedef signed int     sLWord;
typedef unsigned short  Word;
typedef signed short    sWord;
typedef unsigned char   Byte;
typedef signed char     sByte;

typedef unsigned char UCHAR; // for legacy
#include "lib/fatfs/diskio.h"

/******************************************************************************
* Constants
******************************************************************************/

/* SDHC commands */
#define SDHC_CMD0                           (0)
#define SDHC_CMD1                           (1)
#define SDHC_CMD2                           (2)
#define SDHC_CMD3                           (3)
#define SDHC_CMD4                           (4)
#define SDHC_CMD5                           (5)
#define SDHC_CMD6                           (6)
#define SDHC_CMD7                           (7)
#define SDHC_CMD8                           (8)
#define SDHC_CMD9                           (9)
#define SDHC_CMD10                          (10)
#define SDHC_CMD11                          (11)
#define SDHC_CMD12                          (12)
#define SDHC_CMD13                          (13)
#define SDHC_CMD15                          (15)
#define SDHC_CMD16                          (16)
#define SDHC_CMD17                          (17)
#define SDHC_CMD18                          (18)
#define SDHC_CMD20                          (20)
#define SDHC_CMD24                          (24)
#define SDHC_CMD25                          (25)
#define SDHC_CMD26                          (26)
#define SDHC_CMD27                          (27)
#define SDHC_CMD28                          (28)
#define SDHC_CMD29                          (29)
#define SDHC_CMD30                          (30)
#define SDHC_CMD32                          (32)
#define SDHC_CMD33                          (33)
#define SDHC_CMD34                          (34)
#define SDHC_CMD35                          (35)
#define SDHC_CMD36                          (36)
#define SDHC_CMD37                          (37)
#define SDHC_CMD38                          (38)
#define SDHC_CMD39                          (39)
#define SDHC_CMD40                          (40)
#define SDHC_CMD42                          (42)
#define SDHC_CMD52                          (52)
#define SDHC_CMD53                          (53)
#define SDHC_CMD55                          (55)
#define SDHC_CMD56                          (56)
#define SDHC_CMD60                          (60)
#define SDHC_CMD61                          (61)
#define SDHC_ACMD6                          (0x40 + 6)
#define SDHC_ACMD13                         (0x40 + 13)
#define SDHC_ACMD22                         (0x40 + 22)
#define SDHC_ACMD23                         (0x40 + 23)
#define SDHC_ACMD41                         (0x40 + 41)
#define SDHC_ACMD42                         (0x40 + 42)
#define SDHC_ACMD51                         (0x40 + 51)


/* ESDHC command types */
#define SDHC_TYPE_NORMAL                    (0x00)
#define SDHC_TYPE_SUSPEND                   (0x01)
#define SDHC_TYPE_RESUME                    (0x02)
#define SDHC_TYPE_ABORT                     (0x03)
#define SDHC_TYPE_SWITCH_BUSY               (0x04)

#define SDHC_TRANSFERTYPE_DMA               1
#define SDHC_TRANSFERTYPE_SWPOLL            2

#define SDHC_FIFO_BUFFER_SIZE               16
#define SDHC_BLOCK_SIZE                     512

#define SDHC_USE_ISR	                    1	// must always use Interrupts (needed for CMD6)

#define SDHC_DO4BITS                        1	// use 4 bit bus
#define SDHC_TRANSFERTYPE                   SDHC_TRANSFERTYPE_DMA

/******************************************************************************
* Macros 
******************************************************************************/

#define SDHC_ERROR(x,y) {m_sdhc_error = y; sdCardDesc.status = x; return x;}

/******************************************************************************
* Types
******************************************************************************/
typedef struct
{
  DSTATUS status;
  LWord   address;
  Byte    highCapacity;  
  Byte    version2;
  LWord   numBlocks;
  LWord   lastCardStatus;
}SD_CARD_DESCRIPTOR;

extern SD_CARD_DESCRIPTOR sdCardDesc;

typedef enum {
  SD_CARD_ERROR_NONE = 0,

  // Basic commands and switch command.
  SD_CARD_ERROR_CMD0 = 0X20,
  SD_CARD_ERROR_CMD2,
  SD_CARD_ERROR_CMD3,
  SD_CARD_ERROR_CMD6,
  SD_CARD_ERROR_CMD7,
  SD_CARD_ERROR_CMD8,
  SD_CARD_ERROR_CMD9,
  SD_CARD_ERROR_CMD10,
  SD_CARD_ERROR_CMD12,
  SD_CARD_ERROR_CMD13,
  SD_CARD_ERROR_CMD16,

  // Read, write, erase, and extension commands.
  SD_CARD_ERROR_CMD17 = 0X30,
  SD_CARD_ERROR_CMD18,
  SD_CARD_ERROR_CMD24,
  SD_CARD_ERROR_CMD25,
  SD_CARD_ERROR_CMD32,
  SD_CARD_ERROR_CMD33,
  SD_CARD_ERROR_CMD38,
  SD_CARD_ERROR_CMD58,
  SD_CARD_ERROR_CMD59,

  // Application specific commands.
  SD_CARD_ERROR_ACMD6 = 0X40,
  SD_CARD_ERROR_ACMD13,
  SD_CARD_ERROR_ACMD23,
  SD_CARD_ERROR_ACMD41,

  // Misc errors.
  SD_CARD_ERROR_DMA = 0X50,
  SD_CARD_ERROR_ERASE,
  SD_CARD_ERROR_ERASE_SINGLE_BLOCK,
  SD_CARD_ERROR_ERASE_TIMEOUT,
  SD_CARD_ERROR_INIT_NOT_CALLED,
  SD_CARD_ERROR_READ,
  SD_CARD_ERROR_READ_REG,
  SD_CARD_ERROR_READ_TIMEOUT,
  SD_CARD_ERROR_STOP_TRAN,
  SD_CARD_ERROR_WRITE_TIMEOUT,
  SD_CARD_ERROR_WRITE,
} sd_error_code_t;
/******************************************************************************
* Global variables
******************************************************************************/


/******************************************************************************
* Global functions
******************************************************************************/
#ifdef __cplusplus
extern "C"{
#endif

#define RES_NONRSPNS    5 /* 5: No Response */
#define RES_READERROR   6 /* 6: Read Error */
#define RES_WRITEERROR  7 /* 7: Write Error */


DSTATUS SDHC_InitCard(void);
uint16_t SDHC_DMADone(void);
void SDHC_DMAWait(void);

DSTATUS SDHC_GetStatus(void);
LWord SDHC_GetBlockCnt(void);

uint32_t SDHC_Baudrate(void);
//uint32_t SDHC_GetDMAStatus(void);
void SDHC_ClearDMAStatus(void);
//
DRESULT SDHC_ReadBlocks(UCHAR* buff, DWORD sector, UCHAR count);
DRESULT SDHC_WriteBlocks(UCHAR* buff, DWORD sector, UCHAR count);
//
DSTATUS SDHC_CardIsReady(void);
#ifdef __cplusplus
}
#endif

#endif

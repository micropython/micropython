/*
 * This file is part of the Micro Python project, http://micropython.org/
 *
 * Original file from:
 * FatFs - FAT file system module configuration file R0.10c (C)ChaN, 2014
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2013, 2014 Damien P. George
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

/*---------------------------------------------------------------------------/
/---------------------------------------------------------------------------*/

#ifndef _FFCONF
#define _FFCONF 32020	/* Revision ID */

#include "py/mpconfig.h"

/*---------------------------------------------------------------------------/
/ Functions and Buffer Configurations
/---------------------------------------------------------------------------*/

#define	_FS_TINY		1
/* This option switches tiny buffer configuration. (0:Normal or 1:Tiny)
/  At the tiny configuration, size of the file object (FIL) is reduced _MAX_SS
/  bytes. Instead of private sector buffer eliminated from the file object,
/  common sector buffer in the file system object (FATFS) is used for the file
/  data transfer. */


#define _FS_READONLY	0
/* This option switches read-only configuration. (0:Read/Write or 1:Read-only)
/  Read-only configuration removes writing API functions, f_write(), f_sync(),
/  f_unlink(), f_mkdir(), f_chmod(), f_rename(), f_truncate(), f_getfree()
/  and optional writing functions as well. */


#define _FS_MINIMIZE	0
/* This option defines minimization level to remove some basic API functions.
/
/   0: All basic functions are enabled.
/   1: f_stat(), f_getfree(), f_unlink(), f_mkdir(), f_chmod(), f_utime(),
/      f_truncate() and f_rename() function are removed.
/   2: f_opendir(), f_readdir() and f_closedir() are removed in addition to 1.
/   3: f_lseek() function is removed in addition to 2. */


#define	_USE_STRFUNC	0
/* This option switches string functions, f_gets(), f_putc(), f_puts() and
/  f_printf().
/
/  0: Disable string functions.
/  1: Enable without LF-CRLF conversion.
/  2: Enable with LF-CRLF conversion. */


#define _USE_FIND		0
/* This option switches filtered directory read feature and related functions,
/  f_findfirst() and f_findnext(). (0:Disable or 1:Enable) */


#define	_USE_MKFS		1
/* This option switches f_mkfs() function. (0:Disable or 1:Enable) */


#define	_USE_FASTSEEK	0
/* This option switches fast seek feature. (0:Disable or 1:Enable) */

#ifdef MICROPY_FATFS_USE_LABEL
#define _USE_LABEL		(MICROPY_FATFS_USE_LABEL)
#else
#define _USE_LABEL		0
#endif
/* This option switches volume label functions, f_getlabel() and f_setlabel().
/  (0:Disable or 1:Enable) */


#define	_USE_FORWARD	0
/* This option switches f_forward() function. (0:Disable or 1:Enable)
/  To enable it, also _FS_TINY need to be set to 1. */


/*---------------------------------------------------------------------------/
/ Locale and Namespace Configurations
/---------------------------------------------------------------------------*/
#ifdef MICROPY_FATFS_LFN_CODE_PAGE
#define _CODE_PAGE	(MICROPY_FATFS_LFN_CODE_PAGE)
#else
#define _CODE_PAGE	1
#endif
/* This option specifies the OEM code page to be used on the target system.
/  Incorrect setting of the code page can cause a file open failure.
/
/   1    - ASCII (No extended character. Non-LFN cfg. only)
/   437  - U.S.
/   720  - Arabic
/   737  - Greek
/   775  - Baltic
/   850  - Multilingual Latin 1
/   852  - Latin 2
/   855  - Cyrillic
/   857  - Turkish
/   858  - Multilingual Latin 1 + Euro
/   862  - Hebrew
/   866  - Russian
/   874  - Thai
/   932  - Japanese Shift_JIS (DBCS)
/   936  - Simplified Chinese GBK (DBCS)
/   949  - Korean (DBCS)
/   950  - Traditional Chinese Big5 (DBCS)
*/

#ifdef MICROPY_FATFS_ENABLE_LFN
#define	_USE_LFN	(MICROPY_FATFS_ENABLE_LFN)
#else
#define	_USE_LFN	0
#endif
#ifdef MICROPY_FATFS_MAX_LFN
#define	_MAX_LFN	(MICROPY_FATFS_MAX_LFN)
#else
#define	_MAX_LFN	255
#endif
/* The _USE_LFN option switches the LFN feature.
/
/   0: Disable LFN feature. _MAX_LFN has no effect.
/   1: Enable LFN with static working buffer on the BSS. Always NOT thread-safe.
/   2: Enable LFN with dynamic working buffer on the STACK.
/   3: Enable LFN with dynamic working buffer on the HEAP.
/
/  When enable the LFN feature, Unicode handling functions (option/unicode.c) must
/  be added to the project. The LFN working buffer occupies (_MAX_LFN + 1) * 2 bytes.
/  When use stack for the working buffer, take care on stack overflow. When use heap
/  memory for the working buffer, memory management functions, ff_memalloc() and
/  ff_memfree(), must be added to the project. */


#define	_LFN_UNICODE	0
/* This option switches character encoding on the API. (0:ANSI/OEM or 1:Unicode)
/  To use Unicode string for the path name, enable LFN feature and set _LFN_UNICODE
/  to 1. This option also affects behavior of string I/O functions. */


#define _STRF_ENCODE	3
/* When _LFN_UNICODE is 1, this option selects the character encoding on the file to
/  be read/written via string I/O functions, f_gets(), f_putc(), f_puts and f_printf().
/
/  0: ANSI/OEM
/  1: UTF-16LE
/  2: UTF-16BE
/  3: UTF-8
/
/  When _LFN_UNICODE is 0, this option has no effect. */

#ifdef MICROPY_FATFS_RPATH
#define _FS_RPATH	(MICROPY_FATFS_RPATH)
#else
#define _FS_RPATH	0
#endif
/* This option configures relative path feature.
/
/   0: Disable relative path feature and remove related functions.
/   1: Enable relative path feature. f_chdir() and f_chdrive() are available.
/   2: f_getcwd() function is available in addition to 1.
/
/  Note that directory items read via f_readdir() are affected by this option. */


/*---------------------------------------------------------------------------/
/ Drive/Volume Configurations
/---------------------------------------------------------------------------*/

#ifdef MICROPY_FATFS_VOLUMES
#define _VOLUMES	(MICROPY_FATFS_VOLUMES)
#else
#define _VOLUMES	1
#endif
/* Number of volumes (logical drives) to be used. */


#define _STR_VOLUME_ID	0
#define _VOLUME_STRS	"RAM","NAND","CF","SD1","SD2","USB1","USB2","USB3"
/* _STR_VOLUME_ID option switches string volume ID feature.
/  When _STR_VOLUME_ID is set to 1, also pre-defined strings can be used as drive
/  number in the path name. _VOLUME_STRS defines the drive ID strings for each
/  logical drives. Number of items must be equal to _VOLUMES. Valid characters for
/  the drive ID strings are: A-Z and 0-9. */

#ifdef MICROPY_FATFS_MULTI_PARTITION
#define	_MULTI_PARTITION	(MICROPY_FATFS_MULTI_PARTITION)
#else
#define	_MULTI_PARTITION	0
#endif
/* This option switches multi-partition feature. By default (0), each logical drive
/  number is bound to the same physical drive number and only an FAT volume found on
/  the physical drive will be mounted. When multi-partition feature is enabled (1),
/  each logical drive number is bound to arbitrary physical drive and partition
/  listed in the VolToPart[]. Also f_fdisk() funciton will be available. */


#define	_MIN_SS		512
#ifdef MICROPY_FATFS_MAX_SS
#define _MAX_SS		(MICROPY_FATFS_MAX_SS)
#else
#define	_MAX_SS		512
#endif
/* These options configure the range of sector size to be supported. (512, 1024,
/  2048 or 4096) Always set both 512 for most systems, all type of memory cards and
/  harddisk. But a larger value may be required for on-board flash memory and some
/  type of optical media. When _MAX_SS is larger than _MIN_SS, FatFs is configured
/  to variable sector size and GET_SECTOR_SIZE command must be implemented to the
/  disk_ioctl() function. */


#define	_USE_TRIM	0
/* This option switches ATA-TRIM feature. (0:Disable or 1:Enable)
/  To enable Trim feature, also CTRL_TRIM command should be implemented to the
/  disk_ioctl() function. */


#define _FS_NOFSINFO	0
/* If you need to know correct free space on the FAT32 volume, set bit 0 of this
/  option, and f_getfree() function at first time after volume mount will force
/  a full FAT scan. Bit 1 controls the use of last allocated cluster number.
/
/  bit0=0: Use free cluster count in the FSINFO if available.
/  bit0=1: Do not trust free cluster count in the FSINFO.
/  bit1=0: Use last allocated cluster number in the FSINFO if available.
/  bit1=1: Do not trust last allocated cluster number in the FSINFO.
*/



/*---------------------------------------------------------------------------/
/ System Configurations
/---------------------------------------------------------------------------*/

#define _FS_NORTC	0
#define _NORTC_MON	2
#define _NORTC_MDAY	1
#define _NORTC_YEAR	2015
/* The _FS_NORTC option switches timestamp feature. If the system does not have
/  an RTC function or valid timestamp is not needed, set _FS_NORTC to 1 to disable
/  the timestamp feature. All objects modified by FatFs will have a fixed timestamp
/  defined by _NORTC_MON, _NORTC_MDAY and _NORTC_YEAR.
/  When timestamp feature is enabled (_FS_NORTC	== 0), get_fattime() function need
/  to be added to the project to read current time form RTC. _NORTC_MON,
/  _NORTC_MDAY and _NORTC_YEAR have no effect.
/  These options have no effect at read-only configuration (_FS_READONLY == 1). */


#define	_FS_LOCK	0
/* The _FS_LOCK option switches file lock feature to control duplicated file open
/  and illegal operation to open objects. This option must be 0 when _FS_READONLY
/  is 1.
/
/  0:  Disable file lock feature. To avoid volume corruption, application program
/      should avoid illegal open, remove and rename to the open objects.
/  >0: Enable file lock feature. The value defines how many files/sub-directories
/      can be opened simultaneously under file lock control. Note that the file
/      lock feature is independent of re-entrancy. */

#ifdef MICROPY_FATFS_REENTRANT
#define _FS_REENTRANT	(MICROPY_FATFS_REENTRANT)
#else
#define _FS_REENTRANT	0
#endif

// milliseconds
#ifdef MICROPY_FATFS_TIMEOUT
#define _FS_TIMEOUT		(MICROPY_FATFS_TIMEOUT)
#else
#define _FS_TIMEOUT		1000
#endif

#ifdef MICROPY_FATFS_SYNC_T
#define	_SYNC_t			MICROPY_FATFS_SYNC_T
#else
#define	_SYNC_t			HANDLE
#endif
/* The _FS_REENTRANT option switches the re-entrancy (thread safe) of the FatFs
/  module itself. Note that regardless of this option, file access to different
/  volume is always re-entrant and volume control functions, f_mount(), f_mkfs()
/  and f_fdisk() function, are always not re-entrant. Only file/directory access
/  to the same volume is under control of this feature.
/
/   0: Disable re-entrancy. _FS_TIMEOUT and _SYNC_t have no effect.
/   1: Enable re-entrancy. Also user provided synchronization handlers,
/      ff_req_grant(), ff_rel_grant(), ff_del_syncobj() and ff_cre_syncobj()
/      function, must be added to the project. Samples are available in
/      option/syscall.c.
/
/  The _FS_TIMEOUT defines timeout period in unit of time tick.
/  The _SYNC_t defines O/S dependent sync object type. e.g. HANDLE, ID, OS_EVENT*,
/  SemaphoreHandle_t and etc.. */


#define _WORD_ACCESS	0
/* The _WORD_ACCESS option is an only platform dependent option. It defines
/  which access method is used to the word data on the FAT volume.
/
/   0: Byte-by-byte access. Always compatible with all platforms.
/   1: Word access. Do not choose this unless under both the following conditions.
/
/  * Address misaligned memory access is always allowed to ALL instructions.
/  * Byte order on the memory is little-endian.
/
/  If it is the case, _WORD_ACCESS can also be set to 1 to reduce code size.
/  Following table shows allowable settings of some processor types.
/
/   ARM7TDMI    0           ColdFire    0           V850E       0
/   Cortex-M3   0           Z80         0/1         V850ES      0/1
/   Cortex-M0   0           x86         0/1         TLCS-870    0/1
/   AVR         0/1         RX600(LE)   0/1         TLCS-900    0/1
/   AVR32       0           RL78        0           R32C        0
/   PIC18       0/1         SH-2        0           M16C        0/1
/   PIC24       0           H8S         0           MSP430      0
/   PIC32       0           H8/300H     0           8051        0/1
*/

#endif // _FFCONF

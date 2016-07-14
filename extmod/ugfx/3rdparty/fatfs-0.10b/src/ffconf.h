/*---------------------------------------------------------------------------/
/  FatFs - FAT file system module configuration file  R0.10b (C)ChaN, 2014
/---------------------------------------------------------------------------*/

#ifndef _FFCONF
#define _FFCONF 8051	/* Revision ID */


/*---------------------------------------------------------------------------/
/ Functions and Buffer Configurations
/---------------------------------------------------------------------------*/

#define	_FS_TINY		0	/* 0:Normal or 1:Tiny */
/* When _FS_TINY is set to 1, it reduces memory consumption _MAX_SS bytes each
/  file object. For file data transfer, FatFs uses the common sector buffer in
/  the file system object (FATFS) instead of private sector buffer eliminated
/  from the file object (FIL). */


#define _FS_READONLY	0	/* 0:Read/Write or 1:Read only */
/* Setting _FS_READONLY to 1 defines read only configuration. This removes
/  writing functions, f_write(), f_sync(), f_unlink(), f_mkdir(), f_chmod(),
/  f_rename(), f_truncate() and useless f_getfree(). */


#define _FS_MINIMIZE	0	/* 0 to 3 */
/* The _FS_MINIMIZE option defines minimization level to remove API functions.
/
/   0: All basic functions are enabled.
/   1: f_stat(), f_getfree(), f_unlink(), f_mkdir(), f_chmod(), f_utime(),
/      f_truncate() and f_rename() function are removed.
/   2: f_opendir(), f_readdir() and f_closedir() are removed in addition to 1.
/   3: f_lseek() function is removed in addition to 2. */


#define	_USE_STRFUNC	0	/* 0:Disable or 1-2:Enable */
/* To enable string functions, set _USE_STRFUNC to 1 or 2. */


#define	_USE_MKFS		0	/* 0:Disable or 1:Enable */
/* To enable f_mkfs() function, set _USE_MKFS to 1 and set _FS_READONLY to 0 */


#define	_USE_FASTSEEK	0	/* 0:Disable or 1:Enable */
/* To enable fast seek feature, set _USE_FASTSEEK to 1. */


#define _USE_LABEL		0	/* 0:Disable or 1:Enable */
/* To enable volume label functions, set _USE_LAVEL to 1 */


#define	_USE_FORWARD	0	/* 0:Disable or 1:Enable */
/* To enable f_forward() function, set _USE_FORWARD to 1 and set _FS_TINY to 1. */


/*---------------------------------------------------------------------------/
/ Locale and Namespace Configurations
/---------------------------------------------------------------------------*/

#define _CODE_PAGE	932
/* The _CODE_PAGE specifies the OEM code page to be used on the target system.
/  Incorrect setting of the code page can cause a file open failure.
/
/   932  - Japanese Shift_JIS (DBCS, OEM, Windows)
/   936  - Simplified Chinese GBK (DBCS, OEM, Windows)
/   949  - Korean (DBCS, OEM, Windows)
/   950  - Traditional Chinese Big5 (DBCS, OEM, Windows)
/   1250 - Central Europe (Windows)
/   1251 - Cyrillic (Windows)
/   1252 - Latin 1 (Windows)
/   1253 - Greek (Windows)
/   1254 - Turkish (Windows)
/   1255 - Hebrew (Windows)
/   1256 - Arabic (Windows)
/   1257 - Baltic (Windows)
/   1258 - Vietnam (OEM, Windows)
/   437  - U.S. (OEM)
/   720  - Arabic (OEM)
/   737  - Greek (OEM)
/   775  - Baltic (OEM)
/   850  - Multilingual Latin 1 (OEM)
/   858  - Multilingual Latin 1 + Euro (OEM)
/   852  - Latin 2 (OEM)
/   855  - Cyrillic (OEM)
/   866  - Russian (OEM)
/   857  - Turkish (OEM)
/   862  - Hebrew (OEM)
/   874  - Thai (OEM, Windows)
/   1    - ASCII (Valid for only non-LFN configuration) */


#define	_USE_LFN	0		/* 0 to 3 */
#define	_MAX_LFN	255		/* Maximum LFN length to handle (12 to 255) */
/* The _USE_LFN option switches the LFN feature.
/
/   0: Disable LFN feature. _MAX_LFN has no effect.
/   1: Enable LFN with static working buffer on the BSS. Always NOT thread-safe.
/   2: Enable LFN with dynamic working buffer on the STACK.
/   3: Enable LFN with dynamic working buffer on the HEAP.
/
/  When enable LFN feature, Unicode handling functions ff_convert() and ff_wtoupper()
/  function must be added to the project.
/  The LFN working buffer occupies (_MAX_LFN + 1) * 2 bytes. When use stack for the
/  working buffer, take care on stack overflow. When use heap memory for the working
/  buffer, memory management functions, ff_memalloc() and ff_memfree(), must be added
/  to the project. */


#define	_LFN_UNICODE	0	/* 0:ANSI/OEM or 1:Unicode */
/* To switch the character encoding on the FatFs API (TCHAR) to Unicode, enable LFN
/  feature and set _LFN_UNICODE to 1. This option affects behavior of string I/O
/  functions. This option must be 0 when LFN feature is not enabled. */


#define _STRF_ENCODE	3	/* 0:ANSI/OEM, 1:UTF-16LE, 2:UTF-16BE, 3:UTF-8 */
/* When Unicode API is enabled by _LFN_UNICODE option, this option selects the character
/  encoding on the file to be read/written via string I/O functions, f_gets(), f_putc(),
/  f_puts and f_printf(). This option has no effect when _LFN_UNICODE == 0. Note that
/  FatFs supports only BMP. */


#define _FS_RPATH		0	/* 0 to 2 */
/* The _FS_RPATH option configures relative path feature.
/
/   0: Disable relative path feature and remove related functions.
/   1: Enable relative path. f_chdrive() and f_chdir() function are available.
/   2: f_getcwd() function is available in addition to 1.
/
/  Note that output of the f_readdir() fnction is affected by this option. */


/*---------------------------------------------------------------------------/
/ Drive/Volume Configurations
/---------------------------------------------------------------------------*/

#define _VOLUMES	1
/* Number of volumes (logical drives) to be used. */


#define _STR_VOLUME_ID	0	/* 0:Use only 0-9 for drive ID, 1:Use strings for drive ID */
#define _VOLUME_STRS	"RAM","NAND","CF","SD1","SD2","USB1","USB2","USB3"
/* When _STR_VOLUME_ID is set to 1, also pre-defined strings can be used as drive
/  number in the path name. _VOLUME_STRS defines the drive ID strings for each logical
/  drives. Number of items must be equal to _VOLUMES. Valid characters for the drive ID
/  strings are: 0-9 and A-Z. */


#define	_MULTI_PARTITION	0	/* 0:Single partition, 1:Enable multiple partition */
/* By default(0), each logical drive number is bound to the same physical drive number
/  and only a FAT volume found on the physical drive is mounted. When it is set to 1,
/  each logical drive number is bound to arbitrary drive/partition listed in VolToPart[].
*/


#define	_MIN_SS		512
#define	_MAX_SS		512
/* These options configure the range of sector size to be supported. (512, 1024, 2048 or
/  4096) Always set both 512 for most systems, all memory card and harddisk. But a larger
/  value may be required for on-board flash memory and some type of optical media.
/  When _MAX_SS is larger than _MIN_SS, FatFs is configured to variable sector size and
/  GET_SECTOR_SIZE command must be implemented to the disk_ioctl() function. */


#define	_USE_ERASE	0	/* 0:Disable or 1:Enable */
/* To enable sector erase feature, set _USE_ERASE to 1. Also CTRL_ERASE_SECTOR command
/  should be added to the disk_ioctl() function. */


#define _FS_NOFSINFO	0	/* 0 to 3 */
/* If you need to know correct free space on the FAT32 volume, set bit 0 of this option
/  and f_getfree() function at first time after volume mount will force a full FAT scan.
/  Bit 1 controls the last allocated cluster number as bit 0.
/
/  bit0=0: Use free cluster count in the FSINFO if available.
/  bit0=1: Do not trust free cluster count in the FSINFO.
/  bit1=0: Use last allocated cluster number in the FSINFO if available.
/  bit1=1: Do not trust last allocated cluster number in the FSINFO.
*/



/*---------------------------------------------------------------------------/
/ System Configurations
/---------------------------------------------------------------------------*/

#define	_FS_LOCK	0	/* 0:Disable or >=1:Enable */
/* To enable file lock control feature, set _FS_LOCK to non-zero value.
/  The value defines how many files/sub-directories can be opened simultaneously
/  with file lock control. This feature uses bss _FS_LOCK * 12 bytes. */


#define _FS_REENTRANT	0		/* 0:Disable or 1:Enable */
#define _FS_TIMEOUT		1000	/* Timeout period in unit of time tick */
#define	_SYNC_t			HANDLE	/* O/S dependent sync object type. e.g. HANDLE, OS_EVENT*, ID, SemaphoreHandle_t and etc.. */
/* The _FS_REENTRANT option switches the re-entrancy (thread safe) of the FatFs module.
/
/   0: Disable re-entrancy. _FS_TIMEOUT and _SYNC_t have no effect.
/   1: Enable re-entrancy. Also user provided synchronization handlers,
/      ff_req_grant(), ff_rel_grant(), ff_del_syncobj() and ff_cre_syncobj()
/      function must be added to the project.
*/


#define _WORD_ACCESS	0	/* 0 or 1 */
/* The _WORD_ACCESS option is an only platform dependent option. It defines
/  which access method is used to the word data on the FAT volume.
/
/   0: Byte-by-byte access. Always compatible with all platforms.
/   1: Word access. Do not choose this unless under both the following conditions.
/
/  * Address misaligned memory access is always allowed for ALL instructions.
/  * Byte order on the memory is little-endian.
/
/  If it is the case, _WORD_ACCESS can also be set to 1 to improve performance and
/  reduce code size. Following table shows an example of some processor types.
/
/   ARM7TDMI    0           ColdFire    0           V850E       0
/   Cortex-M3   0           Z80         0/1         V850ES      0/1
/   Cortex-M0   0           RX600(LE)   0/1         TLCS-870    0/1
/   AVR         0/1         RX600(BE)   0           TLCS-900    0/1
/   AVR32       0           RL78        0           R32C        0
/   PIC18       0/1         SH-2        0           M16C        0/1
/   PIC24       0           H8S         0           MSP430      0
/   PIC32       0           H8/300H     0           x86         0/1
*/


#endif /* _FFCONF */

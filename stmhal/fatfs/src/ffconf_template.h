/*---------------------------------------------------------------------------/
/  FatFs - FAT file system module configuration file  R0.10  (C)ChaN, 2013
/----------------------------------------------------------------------------/
/
/ CAUTION! Do not forget to make clean the project after any changes to
/ the configuration options.
/
/----------------------------------------------------------------------------*/
#ifndef _FFCONF
#define _FFCONF 80960 /* Revision ID */

/*-----------------------------------------------------------------------------/
/ Additional user header to be used  
/-----------------------------------------------------------------------------*/
/* Replace 'stm32xxx' with the STM32 Serie used, ex: stm32f4xx_hal.h */
#include "stm32xxx_hal.h" 

/* If uSD is used, then include the uSD BSP header file. 
   Replace 'stm32xxx' with your EVAL board name, ex: stm324x9i_eval_sd.h 
   */
#include "stm32xxx_eval_sd.h"

/* If USB Host MSC is used, then need to include the USBH and UBSH MSC core header files */
#include "usbh_core.h"
#include "usbh_msc.h"
/* hUSBH to be updated with the USBH handle defined in the application code */
#define  HOST_HANDLE   hUSBH 

/*-----------------------------------------------------------------------------/
/ Functions and Buffer Configurations
/-----------------------------------------------------------------------------*/

#define _FS_TINY             0      /* 0:Normal or 1:Tiny */
/* When _FS_TINY is set to 1, FatFs uses the sector buffer in the file system
/  object instead of the sector buffer in the individual file object for file
/  data transfer. This reduces memory consumption 512 bytes each file object. */


#define _FS_READONLY         0      /* 0:Read/Write or 1:Read only */
/* Setting _FS_READONLY to 1 defines read only configuration. This removes
/  writing functions, f_write, f_sync, f_unlink, f_mkdir, f_chmod, f_rename,
/  f_truncate and useless f_getfree. */


#define _FS_MINIMIZE         0      /* 0 to 3 */
/* The _FS_MINIMIZE option defines minimization level to remove some functions.
/
/   0: Full function.
/   1: f_stat, f_getfree, f_unlink, f_mkdir, f_chmod, f_truncate, f_utime 
/      and f_rename are removed.
/   2: f_opendir and f_readdir are removed in addition to 1.
/   3: f_lseek is removed in addition to 2. */


#define _USE_STRFUNC         2      /* 0:Disable or 1-2:Enable */
/* To enable string functions, set _USE_STRFUNC to 1 or 2. */


#define _USE_MKFS            1      /* 0:Disable or 1:Enable */
/* To enable f_mkfs function, set _USE_MKFS to 1 and set _FS_READONLY to 0 */


#define _USE_FASTSEEK        1      /* 0:Disable or 1:Enable */
/* To enable fast seek feature, set _USE_FASTSEEK to 1. */


#define _USE_LABEL           0      /* 0:Disable or 1:Enable */
/* To enable volume label functions, set _USE_LAVEL to 1 */


#define _USE_FORWARD         0      /* 0:Disable or 1:Enable */
/* To enable f_forward function, set _USE_FORWARD to 1 and set _FS_TINY to 1. */


/*-----------------------------------------------------------------------------/
/ Local and Namespace Configurations
/-----------------------------------------------------------------------------*/

#define _CODE_PAGE         1252
/* The _CODE_PAGE specifies the OEM code page to be used on the target system.
/  Incorrect setting of the code page can cause a file open failure.
/
/   932  - Japanese Shift-JIS (DBCS, OEM, Windows)
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
/ 1    - ASCII only (Valid for non LFN cfg.)
*/


#define _USE_LFN     3  /* 0 to 3 */
#define _MAX_LFN     255  /* Maximum LFN length to handle (12 to 255) */
/* The _USE_LFN option switches the LFN feature.
/
/   0: Disable LFN feature. _MAX_LFN has no effect.
/   1: Enable LFN with static working buffer on the BSS. Always NOT reentrant.
/   2: Enable LFN with dynamic working buffer on the STACK.
/   3: Enable LFN with dynamic working buffer on the HEAP.
/
/  To enable LFN feature, Unicode handling functions ff_convert() and ff_wtoupper()
/  function must be added to the project.
/  The LFN working buffer occupies (_MAX_LFN + 1) * 2 bytes. When use stack for the
/  working buffer, take care on stack overflow. When use heap memory for the working
/  buffer, memory management functions, ff_memalloc() and ff_memfree(), must be added
/  to the project. */


#define _LFN_UNICODE    0 /* 0:ANSI/OEM or 1:Unicode */
/* To switch the character encoding on the FatFs API to Unicode, enable LFN feature
/  and set _LFN_UNICODE to 1. */


#define _STRF_ENCODE    3 /* 0:ANSI/OEM, 1:UTF-16LE, 2:UTF-16BE, 3:UTF-8 */
/* When Unicode API is enabled, character encoding on the all FatFs API is switched
/  to Unicode. This option selects the character encoding on the file to be read/written
/  via string functions, f_gets(), f_putc(), f_puts and f_printf().
/  This option has no effect when _LFN_UNICODE is 0. */


#define _FS_RPATH       0 /* 0 to 2 */
/* The _FS_RPATH option configures relative path feature.
/
/   0: Disable relative path feature and remove related functions.
/   1: Enable relative path. f_chdrive() and f_chdir() function are available.
/   2: f_getcwd() function is available in addition to 1.
/
/  Note that output of the f_readdir() fnction is affected by this option. */


/*---------------------------------------------------------------------------/
/ Drive/Volume Configurations
/----------------------------------------------------------------------------*/

#define _VOLUMES    1
/* Number of volumes (logical drives) to be used. */


#define _MULTI_PARTITION     0 /* 0:Single partition, 1:Enable multiple partition */
/* When set to 0, each volume is bound to the same physical drive number and
/ it can mount only first primaly partition. When it is set to 1, each volume
/ is tied to the partitions listed in VolToPart[]. */


#define _MAX_SS    512  /* 512, 1024, 2048 or 4096 */
/* Maximum sector size to be handled.
/  Always set 512 for memory card and hard disk but a larger value may be
/  required for on-board flash memory, floppy disk and optical disk.
/  When _MAX_SS is larger than 512, it configures FatFs to variable sector size
/  and GET_SECTOR_SIZE command must be implemented to the disk_ioctl() function. */


#define _USE_ERASE     0 /* 0:Disable or 1:Enable */
/* To enable sector erase feature, set _USE_ERASE to 1. Also CTRL_ERASE_SECTOR command
/  should be added to the disk_ioctl() function. */


#define _FS_NOFSINFO    0 /* 0 or 1 */
/* If you need to know the correct free space on the FAT32 volume, set this
/  option to 1 and f_getfree() function at first time after volume mount will
/  force a full FAT scan.
/
/  0: Load all informations in the FSINFO if available.
/  1: Do not trust free cluster count in the FSINFO.
*/


/*---------------------------------------------------------------------------/
/ System Configurations
/----------------------------------------------------------------------------*/

#define _WORD_ACCESS    0 /* 0 or 1 */
/* The _WORD_ACCESS option is an only platform dependent option. It defines
/  which access method is used to the word data on the FAT volume.
/
/   0: Byte-by-byte access. Always compatible with all platforms.
/   1: Word access. Do not choose this unless under both the following conditions.
/
/  * Byte order on the memory is little-endian.
/  * Address miss-aligned word access is always allowed for all instructions.
/
/  If it is the case, _WORD_ACCESS can also be set to 1 to improve performance
/  and reduce code size.
*/


/* A header file that defines sync object types on the O/S, such as
/  windows.h, ucos_ii.h and semphr.h, must be included prior to ff.h. */

#define _FS_REENTRANT    1  /* 0:Disable or 1:Enable */
#define _FS_TIMEOUT      1000 /* Timeout period in unit of time ticks */
#define _SYNC_t          osSemaphoreId /* O/S dependent type of sync object. e.g. HANDLE, OS_EVENT*, ID and etc.. */

/* The _FS_REENTRANT option switches the re-entrancy (thread safe) of the FatFs module.
/
/   0: Disable re-entrancy. _SYNC_t and _FS_TIMEOUT have no effect.
/   1: Enable re-entrancy. Also user provided synchronization handlers,
/      ff_req_grant(), ff_rel_grant(), ff_del_syncobj() and ff_cre_syncobj()
/      function must be added to the project. */


#define _FS_LOCK    2      /* 0:Disable or >=1:Enable */
/* To enable file lock control feature, set _FS_LOCK to 1 or greater.
   The value defines how many files can be opened simultaneously. */


#endif /* _FFCONFIG */


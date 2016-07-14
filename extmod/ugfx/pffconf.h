/*---------------------------------------------------------------------------/
/  Petit FatFs - Configuration file  R0.03 (C)ChaN, 2014
/---------------------------------------------------------------------------*/

#ifndef _PFFCONF
#define _PFFCONF 4004	/* Revision ID */

/*---------------------------------------------------------------------------/
/ Function Configurations
/---------------------------------------------------------------------------*/

#define	_USE_READ	1	/* Enable pf_read() function */
#define	_USE_DIR	GFILE_NEED_FILELISTS	/* Enable pf_opendir() and pf_readdir() function */
#define	_USE_LSEEK	1	/* Enable pf_lseek() function */
#define	_USE_WRITE	0	/* Enable pf_write() function */

#define _FS_FAT12	0	/* Enable FAT12 */
#define _FS_FAT16	1	/* Enable FAT16 */
#define _FS_FAT32	1	/* Enable FAT32 */


/*---------------------------------------------------------------------------/
/ Locale and Namespace Configurations
/---------------------------------------------------------------------------*/

#define _USE_LCC	0	/* Allow lower case characters for path name */

#define	_CODE_PAGE	437
/* The _CODE_PAGE specifies the code page to be used on the target system.
/  SBCS code pages with _USE_LCC == 1 requiers a 128 byte of case conversion
/  table. This might occupy RAM on some platforms, e.g. avr-gcc.
/  When _USE_LCC == 0, _CODE_PAGE has no effect.
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
*/


/*---------------------------------------------------------------------------/
/ System Configurations
/---------------------------------------------------------------------------*/

#define _WORD_ACCESS	0
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

#endif /* _PFFCONF */

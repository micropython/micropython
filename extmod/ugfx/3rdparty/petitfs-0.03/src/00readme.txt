Petit FatFs Module Source Files R0.03                (C)ChaN, 2014


FILES

  pff.h      Common include file for Petit FatFs and application module.
  pff.c      Petit FatFs module.
  diskio.h   Common include file for Petit FatFs and disk I/O module.
  diskio.c   Skeleton of low level disk I/O module.
  integer.h  Alternative type definitions for integer variables.

  Low level disk I/O module is not included in this archive because the Petit
  FatFs module is only a generic file system layer and not depend on any
  specific storage device. You have to provide a low level disk I/O module that
  written to control your storage device.



AGREEMENTS

 Petit FatFs module is an open source software to implement FAT file system to
 small embedded systems. This is a free software and is opened for education,
 research and commercial developments under license policy of following trems.

  Copyright (C) 2014, ChaN, all right reserved.

 * The Petit FatFs module is a free software and there is NO WARRANTY.
 * No restriction on use. You can use, modify and redistribute it for
   personal, non-profit or commercial use UNDER YOUR RESPONSIBILITY.
 * Redistributions of source code must retain the above copyright notice.



REVISION HISTORY

  Jun 15, 2009  R0.01a  First release (Branched from FatFs R0.07b)

  Dec 14, 2009  R0.02   Added multiple code page support.
                        Added write funciton.
                        Changed stream read mode interface.
  Dec 07,'2010  R0.02a  Added some configuration options.
                        Fixed fails to open objects with DBCS character.

  Jun 10, 2014  R0.03   Separated out configuration options to pffconf.h.
                        Added _USE_LCC option.
                        Added _FS_FAT16 option.

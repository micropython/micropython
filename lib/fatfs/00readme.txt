FatFs Module Source Files R0.11


FILES

  00readme.txt This file.
  history.txt  Revision history.
  ffconf.h     Configuration file for FatFs module.
  ff.h         Common include file for FatFs and application module.
  ff.c         FatFs module.
  diskio.h     Common include file for FatFs and disk I/O module.
  diskio.c     An example of glue function to attach existing disk I/O module to FatFs.
  integer.h    Integer type definitions for FatFs.
  option       Optional external functions.


  Low level disk I/O module is not included in this archive because the FatFs
  module is only a generic file system layer and not depend on any specific
  storage device. You have to provide a low level disk I/O module that written
  to control the target storage device.


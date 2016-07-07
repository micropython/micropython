# This file is subject to the terms of the GFX License. If a copy of
# the license was not distributed with this file, you can obtain one at:
#
#              http://ugfx.org/license.html

GFXSRC +=   $(GFXLIB)/src/gfile/gfile.c \
            $(GFXLIB)/src/gfile/gfile_fs_native.c \
            $(GFXLIB)/src/gfile/gfile_fs_ram.c \
            $(GFXLIB)/src/gfile/gfile_fs_rom.c \
            $(GFXLIB)/src/gfile/gfile_fs_fatfs.c \
            $(GFXLIB)/src/gfile/gfile_fs_petitfs.c \
            $(GFXLIB)/src/gfile/gfile_fs_mem.c \
            $(GFXLIB)/src/gfile/gfile_fs_chibios.c \
            $(GFXLIB)/src/gfile/gfile_fs_strings.c \
            $(GFXLIB)/src/gfile/gfile_printg.c \
            $(GFXLIB)/src/gfile/gfile_scang.c \
            $(GFXLIB)/src/gfile/gfile_stdio.c \
            $(GFXLIB)/src/gfile/gfile_fatfs_wrapper.c \
            $(GFXLIB)/src/gfile/gfile_fatfs_diskio_chibios.c \
            $(GFXLIB)/src/gfile/gfile_petitfs_wrapper.c \
            $(GFXLIB)/src/gfile/gfile_petitfs_diskio_chibios.c \

            
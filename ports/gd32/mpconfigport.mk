# Enable/disable features that the build system needs to know about.
#
# extmod.mk tests these as make variables (not C macros), so anything it
# gates on must be exported here rather than only in mpconfigport.h.

# VFS FAT FS support.  The C side is controlled by MICROPY_VFS_FAT in
# mpconfigport.h; this mirror tells extmod.mk to compile lib/oofatfs.
MICROPY_VFS_FAT ?= 1

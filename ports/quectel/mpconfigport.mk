# Enable/disable modules and 3rd-party libs to be included in interpreter

ifeq ($(strip $(PLAT)), $(filter $(PLAT),Unisoc Unisoc_8910_R05 Unisoc_8910_R06))

MICROPY_VFS_LFS1=1
MICROPY_VFS_LFS2=0

ifeq ($(MICROPY_VFS_LFS_VERSION), 1)
MICROPY_VFS_LFS1=1
MICROPY_VFS_LFS2=0
else
MICROPY_VFS_LFS1=0
MICROPY_VFS_LFS2=1
endif

else

MICROPY_VFS_LFS1=0
MICROPY_VFS_LFS2=1

endif

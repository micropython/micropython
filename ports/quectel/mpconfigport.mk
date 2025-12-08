# Enable/disable modules and 3rd-party libs to be included in interpreter

ifeq ($(strip $(PLAT)), $(filter $(PLAT),EIGEN EIGEN_718))

MICROPY_VFS_LFS1=0
MICROPY_VFS_LFS2=1

else ifeq ($(strip $(PLAT)), $(filter $(PLAT),Unisoc_8850 Unisoc_8850_R02))

MICROPY_VFS_LFS1=1
MICROPY_VFS_LFS2=0

else ifeq ($(strip $(PLAT)), $(filter $(PLAT),Unisoc Unisoc_8910_R05 Unisoc_8910_R06))

MICROPY_VFS_LFS1=1
MICROPY_VFS_LFS2=0

else ifeq ($(strip $(PLAT)), $(filter $(PLAT),SONY SONY_ALT1350))

MICROPY_VFS_LFS1=0
MICROPY_VFS_LFS2=0
MICROPY_VFS_QUECFS=1

else ifeq ($(strip $(PLAT)), $(filter $(PLAT),ASR_1602 ASR_1609))

MICROPY_VFS_LFS1=0
MICROPY_VFS_LFS2=1

else ifeq ($(strip $(PLAT)), $(filter $(PLAT),ASR ASR_1803s ASR_1803sc))

MICROPY_VFS_LFS1=1
MICROPY_VFS_LFS2=0

else ifeq ($(strip $(PLAT)),ASR_1606)

include $(TOP)/../../system/platform/ASR_1606/board/$(BOARD)/mpconfigboard.mk

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

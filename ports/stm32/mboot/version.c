#include "mboot.h"
#include "genhdr/mpversion.h"

#if defined(MBOOT_VERSION_ALLOCATED_BYTES) && MBOOT_VERSION_ALLOCATED_BYTES > 0

#ifndef MBOOT_VERSION
#define MBOOT_VERSION "mboot-" MICROPY_GIT_TAG
#endif

#if MBOOT_VERSION_INCLUDE_OPTIONS  // if this is defined, append a list of build options e.g. fat.lfs2
#define MBOOT_VERSION_USB MBOOT_VERSION "+usb"  // USB is always included

#if defined(MBOOT_I2C_SCL)
#define MBOOT_VERSION_I2C MBOOT_VERSION_USB ".i2c"
#else
#define MBOOT_VERSION_I2C MBOOT_VERSION_USB
#endif

#if MBOOT_ADDRESS_SPACE_64BIT
#define MBOOT_VERSION_64BIT MBOOT_VERSION_I2C ".64"
#else
#define MBOOT_VERSION_64BIT MBOOT_VERSION_I2C
#endif

#if MBOOT_VFS_FAT
#define MBOOT_VERSION_FAT MBOOT_VERSION_64BIT ".fat"
#else
#define MBOOT_VERSION_FAT MBOOT_VERSION_64BIT
#endif

#if MBOOT_VFS_LFS1
#define MBOOT_VERSION_LFS1 MBOOT_VERSION_FAT ".lfs1"
#else
#define MBOOT_VERSION_LFS1 MBOOT_VERSION_FAT
#endif

#if MBOOT_VFS_LFS2
#define MBOOT_VERSION_LFS2 MBOOT_VERSION_LFS1 ".lfs2"
#else
#define MBOOT_VERSION_LFS2 MBOOT_VERSION_LFS1
#endif

#if MBOOT_VFS_RAW
#define MBOOT_VERSION_RAW MBOOT_VERSION_LFS2 ".raw"
#else
#define MBOOT_VERSION_RAW MBOOT_VERSION_LFS2
#endif

#define MBOOT_VERSION_FINAL MBOOT_VERSION_RAW

#else // MBOOT_VERSION_INCLUDE_OPTIONS

#define MBOOT_VERSION_FINAL MBOOT_VERSION

#endif // MBOOT_VERSION_INCLUDE_OPTIONS

// Ensure we don't overrun the allocated space
_Static_assert(sizeof(MBOOT_VERSION_FINAL) <= MBOOT_VERSION_ALLOCATED_BYTES + 1, "mboot version string is too long");
// Cuts off the null terminator
const char mboot_version[sizeof(MBOOT_VERSION_FINAL) - 1] __attribute__((section(".mboot_version"))) __attribute__ ((__used__)) = MBOOT_VERSION_FINAL;

#endif

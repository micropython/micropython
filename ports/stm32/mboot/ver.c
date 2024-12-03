#include "mboot.h"
#include "genhdr/mpversion.h"

#ifdef MBOOT_VER_LEN

#ifndef MBOOT_VER
#define MBOOT_VER "mboot-" MICROPY_GIT_TAG
#endif

#if MBOOT_VER_INC_OPTS  // if this is defined, append a list of build options e.g. fat.lfs2
#define MBOOT_VER_USB MBOOT_VER "+usb"  // USB is always included

#if defined(MBOOT_I2C_SCL)
#define MBOOT_VER_I2C MBOOT_VER_USB ".i2c"
#else
#define MBOOT_VER_I2C MBOOT_VER_USB
#endif

#if MBOOT_ADDRESS_SPACE_64BIT
#define MBOOT_VER_64BIT MBOOT_VER_I2C ".64"
#else
#define MBOOT_VER_64BIT MBOOT_VER_I2C
#endif

#if MBOOT_VFS_FAT
#define MBOOT_VER_FAT MBOOT_VER_64BIT ".fat"
#else
#define MBOOT_VER_FAT MBOOT_VER_64BIT
#endif

#if MBOOT_VFS_LFS1
#define MBOOT_VER_LFS1 MBOOT_VER_FAT ".lfs1"
#else
#define MBOOT_VER_LFS1 MBOOT_VER_FAT
#endif

#if MBOOT_VFS_LFS2
#define MBOOT_VER_LFS2 MBOOT_VER_LFS1 ".lfs2"
#else
#define MBOOT_VER_LFS2 MBOOT_VER_LFS1
#endif

#if MBOOT_VFS_RAW
#define MBOOT_VER_RAW MBOOT_VER_LFS2 ".raw"
#else
#define MBOOT_VER_RAW MBOOT_VER_LFS2
#endif

#define MBOOT_VER_FINAL MBOOT_VER_RAW

#else // MBOOT_VER_INC_OPTS

#define MBOOT_VER_FINAL MBOOT_VER

#endif // MBOOT_VER_INC_OPTS

// Ensure we don't overrun the allocated space
_Static_assert(sizeof(MBOOT_VER_FINAL) <= MBOOT_VER_LEN + 1, "mboot version string is too long");
// Cuts off the null terminator
const char mboot_ver[sizeof(MBOOT_VER_FINAL) - 1] __attribute__((section(".mboot_ver"))) __attribute__ ((__used__)) = MBOOT_VER_FINAL;

#endif  // MBOOT_VER

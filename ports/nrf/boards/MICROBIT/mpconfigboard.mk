MCU_SERIES = m0
MCU_VARIANT = nrf51
MCU_SUB_VARIANT = nrf51822
SOFTDEV_VERSION = 8.0.0

ifneq ($(SD),)
# If SD is used we need to reduce flash use by application
LD_FILES += boards/MICROBIT/nrf51822_smaller_fs.ld
FROZEN_MANIFEST ?=
MICROPY_VFS_LFS2 = 0
else 
ifneq ($(MICROPY_PY_BLUETOOTH),0)
# Otherwise if modbluetooth isn't disabled
# we can need to reduce flash usage a little.
LD_FILES += boards/MICROBIT/nrf51822_smaller_fs.ld
endif
endif
# Enable LFS2 if not disabled above.
MICROPY_VFS_LFS2 ?= 1

LD_FILES += boards/nrf51x22_256k_16k.ld
FLASHER = pyocd

CFLAGS += -DBLUETOOTH_LFCLK_RC

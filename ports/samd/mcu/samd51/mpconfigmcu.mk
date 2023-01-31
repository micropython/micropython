CFLAGS_MCU += -mtune=cortex-m4 -mcpu=cortex-m4 -mfpu=fpv4-sp-d16 -mfloat-abi=hard

MPY_CROSS_MCU_ARCH = armv7m

MICROPY_VFS_LFS2 ?= 1
MICROPY_VFS_FAT ?= 1
FROZEN_MANIFEST ?= mcu/$(MCU_SERIES_LOWER)/manifest.py

SRC_S += shared/runtime/gchelper_thumb2.s

SRC_C += \
	fatfs_port.c \
	drivers/dht/dht.c \

LIBM_SRC_C +=  $(addprefix lib/libm/,\
	acoshf.c \
	asinfacosf.c \
	asinhf.c \
	atan2f.c \
	atanf.c \
	atanhf.c \
	ef_rem_pio2.c \
	erf_lgamma.c \
	fmodf.c \
	kf_cos.c \
	kf_rem_pio2.c \
	kf_sin.c \
	kf_tan.c \
	log1pf.c \
	math.c \
	nearbyintf.c \
	roundf.c \
	sf_cos.c \
	sf_erf.c \
	sf_frexp.c \
	sf_ldexp.c \
	sf_modf.c \
	sf_sin.c \
	sf_tan.c \
	wf_lgamma.c \
	wf_tgamma.c \
	)

srctree = $(ZEPHYR_BASE)

include $(Z_DOTCONFIG)
override ARCH = $(subst $(DQUOTE),,$(CONFIG_ARCH))
SOC_NAME = $(subst $(DQUOTE),,$(CONFIG_SOC))
SOC_SERIES = $(subst $(DQUOTE),,$(CONFIG_SOC_SERIES))
SOC_FAMILY = $(subst $(DQUOTE),,$(CONFIG_SOC_FAMILY))
ifeq ($(SOC_SERIES),)
SOC_PATH = $(SOC_NAME)
else
SOC_PATH = $(SOC_FAMILY)/$(SOC_SERIES)
endif

KBUILD_CFLAGS := -c
include $(ZEPHYR_BASE)/scripts/Kbuild.include

include $(ZEPHYR_BASE)/arch/$(ARCH)/Makefile

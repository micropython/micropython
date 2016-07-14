GFXINC  +=	$(GFXLIB)/boards/base/STM32F746-Discovery
GFXSRC  +=	$(GFXLIB)/boards/base/STM32F746-Discovery/stm32f746g_discovery_sdram.c \
			$(GFXLIB)/boards/base/STM32F746-Discovery/stm32f7_i2c.c
				
ifeq ($(OPT_OS),raw32)
	GFXDEFS +=	STM32F746xx
	GFXSRC	+=	$(STMHAL)/Src/stm32f7xx_hal.c \
				$(STMHAL)/Src/stm32f7xx_hal_cortex.c \
				$(STMHAL)/Src/stm32f7xx_hal_rcc.c \
				$(STMHAL)/Src/stm32f7xx_hal_rcc_ex.h \
				$(STMHAL)/Src/stm32f7xx_hal_gpio.c \
				$(STMHAL)/Src/stm32f7xx_hal_pwr.c \
				$(STMHAL)/Src/stm32f7xx_hal_pwr_ex.c
	GFXSRC	+=	$(GFXLIB)/boards/base/STM32F746-Discovery/stm32f746g_raw32_startup.s \
				$(GFXLIB)/boards/base/STM32F746-Discovery/stm32f746g_raw32_ugfx.c \
				$(GFXLIB)/boards/base/STM32F746-Discovery/stm32f746g_raw32_system.c \
				$(GFXLIB)/boards/base/STM32F746-Discovery/stm32f746g_raw32_interrupts.c
	GFXDEFS	+=	GFX_OS_PRE_INIT_FUNCTION=Raw32OSInit GFX_OS_INIT_NO_WARNING=TRUE
	GFXINC	+=	$(CMSIS)/Device/ST/STM32F7xx/Include \
				$(CMSIS)/Include \
				$(STMHAL)/Inc
	LDSCRIPT = $(GFXLIB)/boards/base/STM32F746-Discovery/stm32f746nghx_flash.ld
endif

include $(GFXLIB)/drivers/gdisp/STM32LTDC/driver.mk
include $(GFXLIB)/drivers/ginput/touch/FT5336/driver.mk
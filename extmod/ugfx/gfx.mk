# This file is subject to the terms of the GFX License. If a copy of
# the license was not distributed with this file, you can obtain one at:
#
#              http://ugfx.org/license.html

GFXINC +=   $(GFXLIB)

ifeq ($(GFXSINGLEMAKE),yes)
	GFXSRC += $(GFXLIB)/src/gfx_mk.c
else
	GFXSRC +=	$(GFXLIB)/src/gfx.c
	include $(GFXLIB)/src/gos/gos.mk
	include $(GFXLIB)/src/gdriver/gdriver.mk
	include $(GFXLIB)/src/gqueue/gqueue.mk
	include $(GFXLIB)/src/gdisp/gdisp.mk
	include $(GFXLIB)/src/gevent/gevent.mk
	include $(GFXLIB)/src/gtimer/gtimer.mk
	include $(GFXLIB)/src/gwin/gwin.mk
	include $(GFXLIB)/src/ginput/ginput.mk
	include $(GFXLIB)/src/gadc/gadc.mk
	include $(GFXLIB)/src/gaudio/gaudio.mk
	include $(GFXLIB)/src/gmisc/gmisc.mk
	include $(GFXLIB)/src/gfile/gfile.mk
	include $(GFXLIB)/src/gtrans/gtrans.mk
endif

# Include the boards and drivers
ifneq ($(GFXBOARD),)
	include $(GFXLIB)/boards/base/$(GFXBOARD)/board.mk
endif
ifneq ($(GFXDRIVERS),)
	include $(patsubst %,$(GFXLIB)/drivers/%/driver.mk,$(GFXDRIVERS))
endif
ifneq ($(GFXDEMO),)
	include $(GFXLIB)/demos/$(GFXDEMO)/demo.mk
endif

# Include the operating system define
ifeq ($(OPT_OS),arduino)
	GFXDEFS += GFX_USE_OS_ARDUINO=TRUE
endif
ifeq ($(OPT_OS),win32)
	GFXDEFS += GFX_USE_OS_WIN32=TRUE
endif
ifeq ($(OPT_OS),win32.chibios)
	GFXDEFS += GFX_USE_OS_CHIBIOS=TRUE
endif
ifeq ($(OPT_OS),win32.raw32)
	GFXDEFS += GFX_USE_OS_RAW32=TRUE
endif
ifeq ($(OPT_OS),linux)
	GFXDEFS += GFX_USE_OS_LINUX=TRUE
endif
ifeq ($(OPT_OS),osx)
	GFXDEFS += GFX_USE_OS_OSX=TRUE
endif
ifeq ($(OPT_OS),chibios)
	GFXDEFS += GFX_USE_OS_CHIBIOS=TRUE
endif
ifeq ($(OPT_OS),freertos)
	GFXDEFS += GFX_USE_OS_FREERTOS=TRUE
endif
ifeq ($(OPT_OS),ecos)
	GFXDEFS += GFX_USE_OS_ECOS=TRUE
endif
ifeq ($(OPT_OS),rawrtos)
	GFXDEFS += GFX_USE_OS_RAWRTOS=TRUE
endif
ifeq ($(OPT_OS),raw32)
	GFXDEFS += GFX_USE_OS_RAW32=TRUE
endif

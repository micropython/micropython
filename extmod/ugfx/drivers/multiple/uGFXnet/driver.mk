GFXINC +=	$(GFXLIB)/drivers/multiple/uGFXnet
GFXSRC +=	$(GFXLIB)/drivers/multiple/uGFXnet/gdisp_lld_uGFXnet.c

ifeq ($(basename $(OPT_OS)),win32)
	GFXLIBS += ws2_32
endif

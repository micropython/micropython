GFXINC += $(GFXLIB)/drivers/gaudio/gadc
GFXSRC += $(GFXLIB)/drivers/gaudio/gadc/gaudio_record_gadc.c

# Make sure the GADC sub-system is turned on
GFXDEFS += -DGFX_USE_GADC=GAUDIO_NEED_RECORD


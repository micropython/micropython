# plat definition

ifeq ($(strip $(PLAT)),)
export PLAT = Unisoc
export BOARD = EG915UEC_AC

DFLAGS = PLAT_$(strip $(PLAT)) BOARD_$(strip $(BOARD)) BOARD=$(strip $(BOARD))
endif


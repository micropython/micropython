BOARD_MICROBIT_DIR = boards/MICROBIT/modules

INC += -I./$(BOARD_MICROBIT_DIR)
CFLAGS += -DBOARD_SPECIFIC_MODULES

SRC_BOARD_MODULES = $(addprefix $(BOARD_MICROBIT_DIR)/,\
                      microbitdisplay.c \
                      microbitimage.c \
                      iters.c \
                      microbitconstimage.c \
                      microbitconstimagetuples.c \
                      modmicrobit.c \
                      )

OBJ += $(addprefix $(BUILD)/, $(SRC_BOARD_MODULES:.c=.o))


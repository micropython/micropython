BOARD_PCA10059_DIR = boards/pca10059/modules

INC += -I./$(BOARD_PCA10059_DIR)
CFLAGS += -DBOARD_SPECIFIC_MODULES

SRC_BOARD_MODULES = $(addprefix $(BOARD_PCA10059_DIR)/,\
                      recover_uicr_regout0.c \
                      )

OBJ += $(addprefix $(BUILD)/, $(SRC_BOARD_MODULES:.c=.o))


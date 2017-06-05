
PCA10040_SRC_C += \
	pca10040_board.c

OBJ += $(addprefix ${BUILD}/boards/PCA10040/, $(PCA10040_SRC_C:.c=.o))

DEFINES += BOARD_PCA10040

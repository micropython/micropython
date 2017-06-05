
D52Q_SRC_C += \
	d52q_board.c

OBJ += $(addprefix ${BUILD}/boards/D52Q/, $(D52Q_SRC_C:.c=.o))

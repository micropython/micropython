MODS_SRC_C += \
	builtin.c

OBJ += $(addprefix ${BUILD}/mods/, $(MODS_SRC_C:.c=.o))

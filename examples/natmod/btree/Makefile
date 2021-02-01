# Location of top-level MicroPython directory
MPY_DIR = ../../..

# Name of module (different to built-in btree so it can coexist)
MOD = btree_$(ARCH)

# Source files (.c or .py)
SRC = btree_c.c btree_py.py

# Architecture to build for (x86, x64, armv7m, xtensa, xtensawin)
ARCH = x64

BTREE_DIR = $(MPY_DIR)/lib/berkeley-db-1.xx
BTREE_DEFS = -D__DBINTERFACE_PRIVATE=1 -Dmpool_error="(void)" -Dabort=abort_ "-Dvirt_fd_t=void*" $(BTREE_DEFS_EXTRA)
CFLAGS += -I$(BTREE_DIR)/PORT/include
CFLAGS += -Wno-old-style-definition -Wno-sign-compare -Wno-unused-parameter $(BTREE_DEFS)

SRC += $(addprefix $(realpath $(BTREE_DIR))/,\
	btree/bt_close.c \
	btree/bt_conv.c \
	btree/bt_delete.c \
	btree/bt_get.c \
	btree/bt_open.c \
	btree/bt_overflow.c \
	btree/bt_page.c \
	btree/bt_put.c \
	btree/bt_search.c \
	btree/bt_seq.c \
	btree/bt_split.c \
	btree/bt_utils.c \
	mpool/mpool.c \
	)

include $(MPY_DIR)/py/dynruntime.mk

# btree needs gnu99 defined
CFLAGS += -std=gnu99

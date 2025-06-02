include variants/coverage/mpconfigvariant.mk

SANITIZER ?= -fsanitize=undefined
CFLAGS += $(SANITIZER)
LDFLAGS += $(SANITIZER)

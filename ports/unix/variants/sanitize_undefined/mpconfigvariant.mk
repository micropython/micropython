include variants/coverage/mpconfigvariant.mk

SANITIZER ?= -fsanitize=undefined -fno-sanitize=nonnull-attribute
CFLAGS += $(SANITIZER)
LDFLAGS += $(SANITIZER)

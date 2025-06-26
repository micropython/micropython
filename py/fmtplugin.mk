CHECKS_PLUGIN :=  $(BUILD)/micropython_checks.so
CFLAGS += -fplugin=$(CHECKS_PLUGIN)
PLUGINDIR := $(shell $(CC) -print-file-name=plugin)/include

ifeq ($(realpath $(PLUGINDIR)/gcc-plugin.h),)
$(error plugin header $(PLUGINDIR)/gcc-plugin.h for $(CC) does not exist)
endif
HOSTCXX ?= g++

.PHONY: plugin
plugin: $(CHECKS_PLUGIN)

$(CHECKS_PLUGIN): $(TOP)/tools/micropython_checks.cc
	$(ECHO) "PLUGIN $@"
	$(Q)$(HOSTCXX)  -o $@ -shared $^ \
		-std=gnu++11 -fPIC -Wall -O -fno-rtti \
		-I$(PLUGINDIR)

# All objects depend on the checks plugin
$(OBJ): $(CHECKS_PLUGIN)

# And so do these very special targets
QSTR_GLOBAL_REQUIREMENTS += $(CHECKS_PLUGIN)

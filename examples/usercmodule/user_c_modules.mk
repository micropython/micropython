MODULES_PATH := ../../examples/usercmodule
MODULES := cexample
MODULES += cppexample
MODULES += subpackage

$(info USER_C_MODULES found in $(MODULES_PATH): $(MODULES))
$(foreach module, $(MODULES), \
    $(eval USERMOD_DIR = $(patsubst %/,%,$(MODULES_PATH))/$(module))\
    $(info Including User C Module from $(USERMOD_DIR))\
	$(eval include $(USERMOD_DIR)/micropython.mk)\
)


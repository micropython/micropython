MODULES := cexample
MODULES += cppexample
MODULES += subpackage

$(foreach module, $(MODULES), \
    $(eval USERMOD_DIR = $(patsubst %/,%,$(USER_C_MODULES_PATH))/$(module))\
    $(info Including User C Module from $(USERMOD_DIR))\
	$(eval include $(USERMOD_DIR)/micropython.mk)\
)


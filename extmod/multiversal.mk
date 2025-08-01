MKAPIFORMAT ?=
# $(eval $(call multiversal_module, module name, defs files))
define multiversal_module
$$(BUILD)/mod$(1).c: $(TOP)/tools/mkapi.py $(filter-out -t,$(2)) $$(DEFS)
	$$(ECHO) "MKAPI $(1)"
	$$(Q)$$(MKDIR) -p $$(BUILD)
	$$(Q)$$(PYTHON) $(TOP)/tools/mkapi.py $(MKAPIFORMAT) -o $$@ -m $(1) $$(TDEFS) $(2)
SRC_C += $(BUILD)/mod$(1).c
SRC_QSTR += $(BUILD)/mod$(1).c
mkapi:: $$(BUILD)/mod$(1).c
endef

SRC_C += extmod/multiverse_support.c
SRC_QSTR += extmod/multiverse_support.c

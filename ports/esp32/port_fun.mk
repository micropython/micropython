# Makefile to create the port-specific functions table for dynamic linking
#
# The port functions table is created by culling function names from the ESP-IDF doxygen
# docs and then massaging that list. Some sections are excluded because referring to all these
# functins pulls them into the build and then iram overflows.
# In addition to the EDP-IDF function a good number of libc functions were pulled in manually.
#
# Running the 'port_fun_new' target in this makefile generates a fresh list and then compares whether
# anything new shows up compared to the existing function list. These must then be added manually as
# desired.

# Start by pulling in common makefile (used to build firmware and native modules)
#MPY_DYNRUNTIME := 1
include Makefile

port_fun_new: $(BUILD)/port_fun_new.inc
	$(Q)if [ ! -s $< ]; then echo "No new functions"; else echo `wc -l <$<`' new functions, see' $<; fi


# List of doxygen-generated include files from the esp-idf documentation from which to cull public
# function names. These files are found in $(ESPIDF)/docs/en/_build/inc/*.inc
# The inclusion of some function sets causes the .iram.text segment to grow and easily
# overflow, this is why only selected files are listed. The "adds" comments indicate how many bytes
# each line adds to iram.text (which has a bit over 4KB free as of MP v1.12 w/IDF v4.0).
PORT_FUN_INC := adc adc_channel can dac dac_channel esp_adc_cal gpio i2c i2s # adds 0
PORT_FUN_INC += esp_efuse esp_err esp_err_defs esp_freertos_hooks esp_flash_encrypt # adds 0
PORT_FUN_INC += esp_idf_version esp_intr_alloc esp_int_wdt esp_ipc esp_mesh # adds 116
PORT_FUN_INC += esp_modbus_common esp_modbus_master esp_modbus_slave # adds 0
PORT_FUN_INC += esp_now esp_ota_ops esp_pm esp_sleep esp_system # adds 12
PORT_FUN_INC += esp_task_wdt esp_timer esp_tls # adds 36
PORT_FUN_INC += esp_wifi esp_wifi_types wifi_config wifi_scan # adds 0
PORT_FUN_INC += ledc mcpwm mdns nvs nvs_flash pcnt pm rmt scratch-build-code sigmadelta # adds 360
PORT_FUN_INC += rtc_gpio_channel rtc_io ulp # adds 0
PORT_FUN_INC += sdio_slave sdmmc_cmd sdmmc_host sdmmc_types sdspi_host spi_common # adds 0
PORT_FUN_INC += timer timers uart # adds 284 (uart)
#PORT_FUN_INC += esp_flash # adds 2192 to iram
#PORT_FUN_INC += multi_heap # adds 484 to iram
#PORT_FUN_INC += spi_master # adds 796 to iram
#PORT_FUN_INC += spi_slave # adds 1148 to iram
#PORT_FUN_INC += task # adds 2124 to iram
#PORT_FUN_INC += touch_channel # adds 444 to iram
PORT_FUN_INC := $(addprefix $(ESPIDF)/docs/en/_build/inc/, $(addsuffix *.inc, $(PORT_FUN_INC)))

# new functions found but not listed in port_fun.inc, typ these are new features added to esp-idf
# and perhaps should be manually added to the end of port_fun.inc
$(BUILD)/port_fun_new.inc: $(BUILD)/port_fun_all.inc port_fun.inc
	$(ECHO) "Generating $@"
	$(Q)diff port_fun.inc $< | grep '^>' | sed -e 's/^> *//' >$@

# all functions mentioned in the esp-idf API reference docs
$(BUILD)/port_fun_docfuns: $(PORT_FUN_INC)
	$(ECHO) "Generating $@"
	$(Q)grep doxygenfunction:: $^ | \
	    egrep -v _Static_assert | \
	    sed -e 's/.*:: *//' -e '/^$$/d' | sort -u >$@

# all exported symbols from esp-idf libraries
$(BUILD)/port_fun_exports: $(BUILD_ESPIDF_LIB)/*/*.a
	$(ECHO) "Generating $@"
	$(Q)xtensa-esp32-elf-nm --extern-only $^ | grep ' T ' | sed -e 's/^.* //' | sort -u > $@

# all functions in the esp-idf docs that are also exported, ready to be #included
$(BUILD)/port_fun_all.inc: $(BUILD)/port_fun_exports $(BUILD)/port_fun_docfuns
	$(ECHO) "Generating $@"
	$(Q)sort $^ | uniq -c | grep '^ *2 ' | sed -e 's/^ *2 //' -e 's/$$/,/' > $@


include ../../py/mkenv.mk

# qstr definitions (must come before including py.mk)
QSTR_DEFS = qstrdefsport.h

MICROPY_PY_USSL = 0
MICROPY_SSL_AXTLS = 0
MICROPY_FATFS = 1
MICROPY_PY_BTREE = 1

#FROZEN_DIR = scripts
FROZEN_MPY_DIR = modules

# include py core make definitions
include $(TOP)/py/py.mk

PORT ?= /dev/ttyUSB0
BAUD ?= 460800
FLASH_MODE ?= dio
FLASH_FREQ ?= 40m
FLASH_SIZE ?= 4MB
CROSS_COMPILE ?= xtensa-esp32-elf-
OBJDUMP = $(CROSS_COMPILE)objdump

# SDKCONFIG should be overridden to get a different configuration
SDKCONFIG ?= boards/sdkconfig
SDKCONFIG_H = $(BUILD)/sdkconfig.h

# the git hash of the currently supported ESP IDF version
ESPIDF_SUPHASH := 6b3da6b1882f3b72e904cc90be67e9c4e3f369a9

# paths to ESP IDF and its components
ifeq ($(ESPIDF),)
ifneq ($(IDF_PATH),)
ESPIDF = $(IDF_PATH)
else
$(info The ESPIDF variable has not been set, please set it to the root of the esp-idf repository.)
$(info See README.md for installation instructions.)
$(info Supported git hash: $(ESPIDF_SUPHASH))
$(error ESPIDF not set)
endif
endif
ESPCOMP = $(ESPIDF)/components
ESPTOOL ?= $(ESPCOMP)/esptool_py/esptool/esptool.py
ESPCOMP_KCONFIGS = $(shell find $(ESPCOMP) -name Kconfig)
ESPCOMP_KCONFIGS_PROJBUILD = $(shell find $(ESPCOMP) -name Kconfig.projbuild)

# verify the ESP IDF version
ESPIDF_CURHASH := $(shell git -C $(ESPIDF) show -s --pretty=format:'%H')
ifneq ($(ESPIDF_CURHASH),$(ESPIDF_SUPHASH))
$(info ** WARNING **)
$(info The git hash of ESP IDF does not match the supported version)
$(info The build may complete and the firmware may work but it is not guaranteed)
$(info ESP IDF path:       $(ESPIDF))
$(info Current git hash:   $(ESPIDF_CURHASH))
$(info Supported git hash: $(ESPIDF_SUPHASH))
endif

# pretty format of ESP IDF version, used internally by the IDF
IDF_VER := $(shell git -C $(ESPIDF) describe)

# include sdkconfig to get needed configuration values
include $(SDKCONFIG)

################################################################################
# Compiler and linker flags

INC += -I.
INC += -I$(TOP)
INC += -I$(TOP)/lib/mp-readline
INC += -I$(TOP)/lib/netutils
INC += -I$(TOP)/lib/timeutils
INC += -I$(BUILD)

INC_ESPCOMP += -I$(ESPCOMP)/bootloader_support/include
INC_ESPCOMP += -I$(ESPCOMP)/bootloader_support/include_bootloader
INC_ESPCOMP += -I$(ESPCOMP)/driver/include
INC_ESPCOMP += -I$(ESPCOMP)/driver/include/driver
INC_ESPCOMP += -I$(ESPCOMP)/nghttp/port/include
INC_ESPCOMP += -I$(ESPCOMP)/nghttp/nghttp2/lib/includes
INC_ESPCOMP += -I$(ESPCOMP)/efuse/include
INC_ESPCOMP += -I$(ESPCOMP)/efuse/esp32/include
INC_ESPCOMP += -I$(ESPCOMP)/esp32/include
INC_ESPCOMP += -I$(ESPCOMP)/espcoredump/include
INC_ESPCOMP += -I$(ESPCOMP)/soc/include
INC_ESPCOMP += -I$(ESPCOMP)/soc/esp32/include
INC_ESPCOMP += -I$(ESPCOMP)/ethernet/include
INC_ESPCOMP += -I$(ESPCOMP)/expat/expat/expat/lib
INC_ESPCOMP += -I$(ESPCOMP)/expat/port/include
INC_ESPCOMP += -I$(ESPCOMP)/heap/include
INC_ESPCOMP += -I$(ESPCOMP)/json/include
INC_ESPCOMP += -I$(ESPCOMP)/json/port/include
INC_ESPCOMP += -I$(ESPCOMP)/log/include
INC_ESPCOMP += -I$(ESPCOMP)/newlib/platform_include
INC_ESPCOMP += -I$(ESPCOMP)/newlib/include
INC_ESPCOMP += -I$(ESPCOMP)/nvs_flash/include
INC_ESPCOMP += -I$(ESPCOMP)/freertos/include
INC_ESPCOMP += -I$(ESPCOMP)/esp_ringbuf/include
INC_ESPCOMP += -I$(ESPCOMP)/esp_event/include
INC_ESPCOMP += -I$(ESPCOMP)/tcpip_adapter/include
INC_ESPCOMP += -I$(ESPCOMP)/lwip/lwip/src/include
INC_ESPCOMP += -I$(ESPCOMP)/lwip/port/esp32/include
INC_ESPCOMP += -I$(ESPCOMP)/lwip/include/apps
INC_ESPCOMP += -I$(ESPCOMP)/mbedtls/mbedtls/include
INC_ESPCOMP += -I$(ESPCOMP)/mbedtls/port/include
INC_ESPCOMP += -I$(ESPCOMP)/spi_flash/include
INC_ESPCOMP += -I$(ESPCOMP)/ulp/include
INC_ESPCOMP += -I$(ESPCOMP)/vfs/include
INC_ESPCOMP += -I$(ESPCOMP)/xtensa-debug-module/include
INC_ESPCOMP += -I$(ESPCOMP)/wpa_supplicant/include
INC_ESPCOMP += -I$(ESPCOMP)/wpa_supplicant/port/include
INC_ESPCOMP += -I$(ESPCOMP)/ethernet/include
INC_ESPCOMP += -I$(ESPCOMP)/app_trace/include
INC_ESPCOMP += -I$(ESPCOMP)/app_update/include
INC_ESPCOMP += -I$(ESPCOMP)/pthread/include
INC_ESPCOMP += -I$(ESPCOMP)/smartconfig_ack/include
INC_ESPCOMP += -I$(ESPCOMP)/sdmmc/include

# these flags are common to C and C++ compilation
CFLAGS_COMMON = -Os -ffunction-sections -fdata-sections -fstrict-volatile-bitfields \
	-mlongcalls -nostdlib \
	-Wall -Werror -Wno-error=unused-function -Wno-error=unused-but-set-variable \
	-Wno-error=unused-variable -Wno-error=deprecated-declarations \
	-DESP_PLATFORM

CFLAGS_BASE = -std=gnu99 $(CFLAGS_COMMON) -DMBEDTLS_CONFIG_FILE='"mbedtls/esp_config.h"' -DHAVE_CONFIG_H
CFLAGS = $(CFLAGS_BASE) $(INC) $(INC_ESPCOMP)
CFLAGS += -DIDF_VER=\"$(IDF_VER)\"
CFLAGS += $(CFLAGS_MOD)

# this is what ESPIDF uses for c++ compilation
CXXFLAGS = -std=gnu++11 $(CFLAGS_COMMON) $(INC) $(INC_ESPCOMP)

LDFLAGS = -nostdlib -Map=$(@:.elf=.map) --cref
LDFLAGS += --gc-sections -static -EL
LDFLAGS += -u call_user_start_cpu0 -u uxTopUsedPriority -u ld_include_panic_highint_hdl
LDFLAGS += -u __cxa_guard_dummy # so that implementation of static guards is taken from cxx_guards.o instead of libstdc++.a
LDFLAGS += -L$(ESPCOMP)/esp32/ld
LDFLAGS += -T $(BUILD)/esp32_out.ld
LDFLAGS += -T $(BUILD)/esp32.project.ld
LDFLAGS += -T esp32.rom.ld
LDFLAGS += -T esp32.rom.libgcc.ld
LDFLAGS += -T esp32.peripherals.ld

LIBGCC_FILE_NAME = $(shell $(CC) $(CFLAGS) -print-libgcc-file-name)
LIBSTDCXX_FILE_NAME = $(shell $(CXX) $(CXXFLAGS) -print-file-name=libstdc++.a)

# Debugging/Optimization
ifeq ($(DEBUG), 1)
CFLAGS += -g
COPT = -O0
else
#CFLAGS += -fdata-sections -ffunction-sections
COPT += -Os -DNDEBUG
#LDFLAGS += --gc-sections
endif

# Enable SPIRAM support if CONFIG_SPIRAM_SUPPORT=y in sdkconfig
ifeq ($(CONFIG_SPIRAM_SUPPORT),y)
CFLAGS_COMMON += -mfix-esp32-psram-cache-issue
LIBC_LIBM = $(ESPCOMP)/newlib/lib/libc-psram-workaround.a $(ESPCOMP)/newlib/lib/libm-psram-workaround.a
else
LDFLAGS += -T esp32.rom.spiram_incompatible_fns.ld
LIBC_LIBM = $(ESPCOMP)/newlib/lib/libc.a $(ESPCOMP)/newlib/lib/libm.a
endif

################################################################################
# List of MicroPython source and object files

SRC_C = \
	main.c \
	uart.c \
	gccollect.c \
	mphalport.c \
	fatfs_port.c \
	help.c \
	modutime.c \
	moduos.c \
	machine_timer.c \
	machine_pin.c \
	machine_touchpad.c \
	machine_adc.c \
	machine_dac.c \
	machine_pwm.c \
	machine_uart.c \
	modmachine.c \
	modnetwork.c \
	network_lan.c \
	network_ppp.c \
	modsocket.c \
	modesp.c \
	esp32_ulp.c \
	modesp32.c \
	espneopixel.c \
	machine_hw_spi.c \
	machine_wdt.c \
	mpthreadport.c \
	machine_rtc.c \
	machine_sdcard.c \
	$(SRC_MOD)

EXTMOD_SRC_C = $(addprefix extmod/,\
	modonewire.c \
	)

LIB_SRC_C = $(addprefix lib/,\
	mp-readline/readline.c \
	netutils/netutils.c \
	timeutils/timeutils.c \
	utils/pyexec.c \
	utils/interrupt_char.c \
	utils/sys_stdio_mphal.c \
	)

ifeq ($(MICROPY_FATFS), 1)
LIB_SRC_C += \
	lib/oofatfs/ff.c \
	lib/oofatfs/ffunicode.c
endif

DRIVERS_SRC_C = $(addprefix drivers/,\
	bus/softspi.c \
	dht/dht.c \
	)

OBJ_MP =
OBJ_MP += $(PY_O)
OBJ_MP += $(addprefix $(BUILD)/, $(SRC_C:.c=.o))
OBJ_MP += $(addprefix $(BUILD)/, $(EXTMOD_SRC_C:.c=.o))
OBJ_MP += $(addprefix $(BUILD)/, $(LIB_SRC_C:.c=.o))
OBJ_MP += $(addprefix $(BUILD)/, $(DRIVERS_SRC_C:.c=.o))

# List of sources for qstr extraction
SRC_QSTR += $(SRC_C) $(EXTMOD_SRC_C) $(LIB_SRC_C) $(DRIVERS_SRC_C)
# Append any auto-generated sources that are needed by sources listed in SRC_QSTR
SRC_QSTR_AUTO_DEPS +=

################################################################################
# Generate sdkconfig.h from sdkconfig

$(SDKCONFIG_H): $(SDKCONFIG)
	$(ECHO) "GEN $@"
	$(Q)$(MKDIR) -p $(dir $@)
	$(Q)$(PYTHON) $(ESPIDF)/tools/kconfig_new/confgen.py \
		--output header $@ \
		--config $< \
		--kconfig $(ESPIDF)/Kconfig \
		--env "IDF_TARGET=esp32" \
		--env "IDF_CMAKE=n" \
		--env "COMPONENT_KCONFIGS=$(ESPCOMP_KCONFIGS)" \
		--env "COMPONENT_KCONFIGS_PROJBUILD=$(ESPCOMP_KCONFIGS_PROJBUILD)"
	$(Q)touch $@

$(HEADER_BUILD)/qstrdefs.generated.h: $(SDKCONFIG_H)

################################################################################
# List of object files from the ESP32 IDF components

ESPIDF_DRIVER_O = $(subst .c,.o,$(wildcard $(ESPCOMP)/driver/*.c))

ESPIDF_EFUSE_O = $(addprefix $(ESPCOMP)/efuse/,\
	esp32/esp_efuse_table.o \
	src/esp_efuse_api.o \
	src/esp_efuse_utility.o \
	)

$(BUILD)/$(ESPCOMP)/esp32/dport_access.o: CFLAGS += -Wno-array-bounds
ESPIDF_ESP32_O = $(addprefix $(ESPCOMP)/esp32/,\
	brownout.o \
	cache_sram_mmu.o \
	coexist.o \
	dbg_stubs.o \
	dport_panic_highint_hdl.o \
	esp_adapter.o \
	esp_err_to_name.o \
	esp_himem.o \
	panic.o \
	pm_trace.o \
	reset_reason.o \
	restore.o \
	stack_check.o \
	esp_timer.o \
	esp_timer_esp32.o \
	ets_timer_legacy.o \
	event_default_handlers.o \
	fast_crypto_ops.o \
	task_wdt.o \
	cache_err_int.o \
	clk.o \
	cpu_start.o \
	gdbstub.o \
	crosscore_int.o \
	ipc.o \
	int_wdt.o \
	event_loop.o \
	hwcrypto/sha.o \
	hwcrypto/aes.o \
	lib_printf.o \
	freertos_hooks.o \
	system_api.o \
	hw_random.o \
	phy_init.o \
	pm_esp32.o \
	pm_locks.o \
	intr_alloc.o \
	dport_access.o \
	wifi_init.o \
	sleep_modes.o \
	spiram.o \
	spiram_psram.o \
	)

ESPIDF_ESP_RINGBUF_O = $(addprefix $(ESPCOMP)/esp_ringbuf/,\
	ringbuf.o \
	)

ESPIDF_HEAP_O = $(addprefix $(ESPCOMP)/heap/,\
	heap_caps.o \
	heap_caps_init.o \
	multi_heap.o \
	)

ESPIDF_SOC_O = $(addprefix $(ESPCOMP)/soc/,\
	esp32/cpu_util.o \
	esp32/gpio_periph.o \
	esp32/rtc_clk.o \
	esp32/rtc_init.o \
	esp32/rtc_periph.o \
	esp32/rtc_pm.o \
	esp32/rtc_sleep.o \
	esp32/rtc_time.o \
	esp32/rtc_wdt.o \
	esp32/sdmmc_periph.o \
	esp32/soc_memory_layout.o \
	esp32/spi_periph.o \
	src/memory_layout_utils.o \
	)

ESPIDF_CXX_O = $(addprefix $(ESPCOMP)/cxx/,\
	cxx_guards.o \
	)

ESPIDF_ETHERNET_O = $(addprefix $(ESPCOMP)/ethernet/,\
	emac_dev.o \
	emac_main.o \
	eth_phy/phy_tlk110.o \
	eth_phy/phy_lan8720.o \
	eth_phy/phy_common.o \
	)

$(BUILD)/$(ESPCOMP)/expat/%.o: CFLAGS += -DHAVE_EXPAT_CONFIG_H -DHAVE_GETRANDOM
ESPIDF_EXPAT_O = $(addprefix $(ESPCOMP)/expat/,\
	expat/expat/lib/xmltok_ns.o \
	expat/expat/lib/xmltok.o \
	expat/expat/lib/xmlparse.o \
	expat/expat/lib/xmlrole.o \
	expat/expat/lib/xmltok_impl.o \
	)

ESPIDF_PTHREAD_O = $(addprefix $(ESPCOMP)/pthread/,\
	pthread.o \
	pthread_local_storage.o \
	)

# Assembler .S files need only basic flags, and in particular should not have
# -Os because that generates subtly different code.
# We also need custom CFLAGS for .c files because FreeRTOS has headers with
# generic names (eg queue.h) which can clash with other files in the port.
CFLAGS_ASM = -I$(BUILD) -I$(ESPCOMP)/esp32/include -I$(ESPCOMP)/soc/esp32/include -I$(ESPCOMP)/freertos/include/freertos -I.
$(BUILD)/$(ESPCOMP)/freertos/portasm.o: CFLAGS = $(CFLAGS_ASM)
$(BUILD)/$(ESPCOMP)/freertos/xtensa_context.o: CFLAGS = $(CFLAGS_ASM)
$(BUILD)/$(ESPCOMP)/freertos/xtensa_intr_asm.o: CFLAGS = $(CFLAGS_ASM)
$(BUILD)/$(ESPCOMP)/freertos/xtensa_vectors.o: CFLAGS = $(CFLAGS_ASM)
$(BUILD)/$(ESPCOMP)/freertos/xtensa_vector_defaults.o: CFLAGS = $(CFLAGS_ASM)
$(BUILD)/$(ESPCOMP)/freertos/%.o: CFLAGS = $(CFLAGS_BASE) -I. -I$(BUILD) $(INC_ESPCOMP) -I$(ESPCOMP)/freertos/include/freertos -D_ESP_FREERTOS_INTERNAL
ESPIDF_FREERTOS_O = $(addprefix $(ESPCOMP)/freertos/,\
	croutine.o \
	event_groups.o \
	FreeRTOS-openocd.o \
	list.o \
	portasm.o \
	port.o \
	queue.o \
	tasks.o \
	timers.o \
	xtensa_context.o \
	xtensa_init.o \
	xtensa_intr_asm.o \
	xtensa_intr.o \
	xtensa_overlay_os_hook.o \
	xtensa_vector_defaults.o \
	xtensa_vectors.o \
	)

ESPIDF_VFS_O = $(addprefix $(ESPCOMP)/vfs/,\
	vfs_uart.o \
	vfs.o \
	)

ESPIDF_JSON_O = $(addprefix $(ESPCOMP)/json/cJSON/,\
	cJSON.o \
	cJSON_Utils.o \
	)

ESPIDF_LOG_O = $(addprefix $(ESPCOMP)/log/,\
	log.o \
	)

ESPIDF_XTENSA_DEBUG_MODULE_O = $(addprefix $(ESPCOMP)/xtensa-debug-module/,\
	eri.o \
	trax.o \
	)

ESPIDF_TCPIP_ADAPTER_O = $(addprefix $(ESPCOMP)/tcpip_adapter/,\
	tcpip_adapter_lwip.o \
	)

ESPIDF_APP_TRACE_O = $(addprefix $(ESPCOMP)/app_trace/,\
	app_trace.o \
	)

ESPIDF_APP_UPDATE_O = $(addprefix $(ESPCOMP)/app_update/,\
	esp_app_desc.o \
	esp_ota_ops.o \
	)

ESPIDF_NEWLIB_O = $(addprefix $(ESPCOMP)/newlib/,\
	time.o \
	select.o \
	syscalls.o \
	syscall_table.o \
	reent_init.o \
	locks.o \
	)

ESPIDF_NGHTTP_O = $(addprefix $(ESPCOMP)/nghttp/,\
	nghttp2/lib/nghttp2_http.o \
	nghttp2/lib/nghttp2_version.o \
	nghttp2/lib/nghttp2_mem.o \
	nghttp2/lib/nghttp2_hd_huffman.o \
	nghttp2/lib/nghttp2_rcbuf.o \
	nghttp2/lib/nghttp2_callbacks.o \
	nghttp2/lib/nghttp2_session.o \
	nghttp2/lib/nghttp2_stream.o \
	nghttp2/lib/nghttp2_hd.o \
	nghttp2/lib/nghttp2_priority_spec.o \
	nghttp2/lib/nghttp2_buf.o \
	nghttp2/lib/nghttp2_option.o \
	nghttp2/lib/nghttp2_npn.o \
	nghttp2/lib/nghttp2_helper.o \
	nghttp2/lib/nghttp2_frame.o \
	nghttp2/lib/nghttp2_outbound_item.o \
	nghttp2/lib/nghttp2_hd_huffman_data.o \
	nghttp2/lib/nghttp2_pq.o \
	nghttp2/lib/nghttp2_queue.o \
	nghttp2/lib/nghttp2_submit.o \
	nghttp2/lib/nghttp2_map.o \
	port/http_parser.o \
	)

ESPIDF_NVS_FLASH_O = $(addprefix $(ESPCOMP)/nvs_flash/,\
	src/nvs_ops.o \
	src/nvs_types.o \
	src/nvs_page.o \
	src/nvs_item_hash_list.o \
	src/nvs_pagemanager.o \
	src/nvs_storage.o \
	src/nvs_api.o \
	)

ESPIDF_OPENSSL_O = $(addprefix $(ESPCOMP)/openssl/,\
	)

ESPIDF_SMARTCONFIG_ACK_O = $(addprefix $(ESPCOMP)/smartconfig_ack/,\
	smartconfig_ack.o \
	)

ESPIDF_SPI_FLASH_O = $(addprefix $(ESPCOMP)/spi_flash/,\
	flash_mmap.o \
	partition.o \
	spi_flash_rom_patch.o \
	cache_utils.o \
	flash_ops.o \
	)

ESPIDF_ULP_O = $(addprefix $(ESPCOMP)/ulp/,\
	ulp.o \
	)

$(BUILD)/$(ESPCOMP)/lwip/%.o: CFLAGS += -Wno-address -Wno-unused-variable -Wno-unused-but-set-variable
ESPIDF_LWIP_O = $(addprefix $(ESPCOMP)/lwip/,\
	apps/dhcpserver/dhcpserver.o \
	lwip/src/api/netbuf.o \
	lwip/src/api/api_lib.o \
	lwip/src/api/netifapi.o \
	lwip/src/api/tcpip.o \
	lwip/src/api/netdb.o \
	lwip/src/api/err.o \
	lwip/src/api/api_msg.o \
	lwip/src/api/sockets.o \
	lwip/src/apps/sntp/sntp.o \
	lwip/src/core/ipv4/dhcp.o \
	lwip/src/core/ipv4/etharp.o \
	lwip/src/core/ipv4/ip4_addr.o \
	lwip/src/core/ipv4/igmp.o \
	lwip/src/core/ipv4/ip4.o \
	lwip/src/core/ipv4/autoip.o \
	lwip/src/core/ipv4/icmp.o \
	lwip/src/core/ipv6/ip6_frag.o \
	lwip/src/core/ipv6/dhcp6.o \
	lwip/src/core/ipv6/inet6.o \
	lwip/src/core/ipv6/ip6_addr.o \
	lwip/src/core/ipv6/ip6.o \
	lwip/src/core/ipv6/nd6.o \
	lwip/src/core/ipv6/mld6.o \
	lwip/src/core/ipv6/ethip6.o \
	lwip/src/core/ipv6/icmp6.o \
	lwip/src/core/mem.o \
	lwip/src/core/init.o \
	lwip/src/core/memp.o \
	lwip/src/core/sys.o \
	lwip/src/core/tcp_in.o \
	lwip/src/core/timeouts.o \
	lwip/src/core/dns.o \
	lwip/src/core/ip.o \
	lwip/src/core/pbuf.o \
	lwip/src/core/raw.o \
	lwip/src/core/tcp.o \
	lwip/src/core/def.o \
	lwip/src/core/netif.o \
	lwip/src/core/stats.o \
	lwip/src/core/inet_chksum.o \
	lwip/src/core/udp.o \
	lwip/src/core/tcp_out.o \
	lwip/src/netif/slipif.o \
	lwip/src/netif/ethernet.o \
	lwip/src/netif/lowpan6.o \
	lwip/src/netif/ethernetif.o \
	lwip/src/netif/ppp/auth.o \
	lwip/src/netif/ppp/chap-md5.o \
	lwip/src/netif/ppp/chap-new.o \
	lwip/src/netif/ppp/fsm.o \
	lwip/src/netif/ppp/ipcp.o \
	lwip/src/netif/ppp/ipv6cp.o \
	lwip/src/netif/ppp/lcp.o \
	lwip/src/netif/ppp/magic.o \
	lwip/src/netif/ppp/polarssl/md5.o \
	lwip/src/netif/ppp/ppp.o \
	lwip/src/netif/ppp/pppapi.o \
	lwip/src/netif/ppp/pppos.o \
	lwip/src/netif/ppp/upap.o \
	lwip/src/netif/ppp/utils.o \
	lwip/src/netif/ppp/vj.o \
	port/esp32/freertos/sys_arch.o \
	port/esp32/netif/ethernetif.o \
	port/esp32/netif/wlanif.o \
	port/esp32/vfs_lwip.o \
	)

ESPIDF_MBEDTLS_O = $(addprefix $(ESPCOMP)/mbedtls/,\
	mbedtls/library/entropy.o \
	mbedtls/library/pkcs12.o \
	mbedtls/library/ccm.o \
	mbedtls/library/pk.o \
	mbedtls/library/sha1.o \
	mbedtls/library/x509_csr.o \
	mbedtls/library/ssl_cli.o \
	mbedtls/library/ecp.o \
	mbedtls/library/blowfish.o \
	mbedtls/library/x509.o \
	mbedtls/library/ecp_curves.o \
	mbedtls/library/error.o \
	mbedtls/library/ssl_ticket.o \
	mbedtls/library/entropy_poll.o \
	mbedtls/library/cipher.o \
	mbedtls/library/version_features.o \
	mbedtls/library/ripemd160.o \
	mbedtls/library/rsa.o \
	mbedtls/library/rsa_internal.o \
	mbedtls/library/md.o \
	mbedtls/library/md_wrap.o \
	mbedtls/library/sha256.o \
	mbedtls/library/dhm.o \
	mbedtls/library/ssl_cache.o \
	mbedtls/library/pkwrite.o \
	mbedtls/library/base64.o \
	mbedtls/library/asn1parse.o \
	mbedtls/library/ssl_tls.o \
	mbedtls/library/hmac_drbg.o \
	mbedtls/library/pem.o \
	mbedtls/library/version.o \
	mbedtls/library/gcm.o \
	mbedtls/library/memory_buffer_alloc.o \
	mbedtls/library/md2.o \
	mbedtls/library/ecdsa.o \
	mbedtls/library/ssl_srv.o \
	mbedtls/library/x509_crt.o \
	mbedtls/library/ecdh.o \
	mbedtls/library/asn1write.o \
	mbedtls/library/md4.o \
	mbedtls/library/debug.o \
	mbedtls/library/x509_create.o \
	mbedtls/library/ecjpake.o \
	mbedtls/library/oid.o \
	mbedtls/library/md5.o \
	mbedtls/library/ssl_ciphersuites.o \
	mbedtls/library/sha512.o \
	mbedtls/library/xtea.o \
	mbedtls/library/aes.o \
	mbedtls/library/cipher_wrap.o \
	mbedtls/library/arc4.o \
	mbedtls/library/bignum.o \
	mbedtls/library/pkparse.o \
	mbedtls/library/padlock.o \
	mbedtls/library/threading.o \
	mbedtls/library/x509_crl.o \
	mbedtls/library/pkcs11.o \
	mbedtls/library/aesni.o \
	mbedtls/library/timing.o \
	mbedtls/library/certs.o \
	mbedtls/library/pkcs5.o \
	mbedtls/library/ssl_cookie.o \
	mbedtls/library/camellia.o \
	mbedtls/library/havege.o \
	mbedtls/library/des.o \
	mbedtls/library/x509write_csr.o \
	mbedtls/library/platform.o \
	mbedtls/library/platform_util.o \
	mbedtls/library/ctr_drbg.o \
	mbedtls/library/x509write_crt.o \
	mbedtls/library/pk_wrap.o \
	port/esp_bignum.o \
	port/esp_hardware.o \
	port/esp_mem.o \
	port/esp_sha1.o \
	port/esp_sha256.o \
	port/esp_sha512.o \
	)

$(BUILD)/$(ESPCOMP)/wpa_supplicant/%.o: CFLAGS += -DEMBEDDED_SUPP -DIEEE8021X_EAPOL -DEAP_PEER_METHOD -DEAP_MSCHAPv2 -DEAP_TTLS -DEAP_TLS -DEAP_PEAP -DUSE_WPA2_TASK -DCONFIG_WPS2 -DCONFIG_WPS_PIN -DUSE_WPS_TASK -DESPRESSIF_USE -DESP32_WORKAROUND -D__ets__ -Wno-strict-aliasing
ESPIDF_WPA_SUPPLICANT_O = $(addprefix $(ESPCOMP)/wpa_supplicant/,\
	src/crypto/aes-internal-enc.o \
	src/crypto/sha256-internal.o \
	src/crypto/md5-internal.o \
	src/crypto/aes-internal.o \
	src/crypto/sha1.o \
	src/crypto/aes-internal-dec.o \
	src/crypto/aes-unwrap.o \
	src/crypto/crypto_internal-rsa.o \
	src/crypto/dh_groups.o \
	src/crypto/crypto_internal.o \
	src/crypto/aes-wrap.o \
	src/crypto/sha1-internal.o \
	src/crypto/dh_group5.o \
	src/crypto/sha256.o \
	src/crypto/rc4.o \
	src/crypto/md5.o \
	src/crypto/aes-cbc.o \
	src/crypto/sha1-pbkdf2.o \
	src/crypto/bignum.o \
	src/crypto/crypto_internal-modexp.o \
	src/crypto/crypto_internal-cipher.o \
	src/fast_crypto/fast_aes-unwrap.o \
	src/fast_crypto/fast_aes-wrap.o \
	src/fast_crypto/fast_sha256.o \
	src/fast_crypto/fast_sha256-internal.o \
	port/os_xtensa.o \
	)

ESPIDF_SDMMC_O = $(addprefix $(ESPCOMP)/sdmmc/,\
	sdmmc_cmd.o \
	sdmmc_common.o \
	sdmmc_init.o \
	sdmmc_mmc.o \
	sdmmc_sd.o \
	sdmmc_io.o \
	)

OBJ_ESPIDF =
OBJ_ESPIDF += $(addprefix $(BUILD)/, $(ESPIDF_NEWLIB_O))
OBJ_ESPIDF += $(addprefix $(BUILD)/, $(ESPIDF_DRIVER_O))
OBJ_ESPIDF += $(addprefix $(BUILD)/, $(ESPIDF_EFUSE_O))
OBJ_ESPIDF += $(addprefix $(BUILD)/, $(ESPIDF_ESP32_O))
OBJ_ESPIDF += $(addprefix $(BUILD)/, $(ESPIDF_ESP_RINGBUF_O))
OBJ_ESPIDF += $(addprefix $(BUILD)/, $(ESPIDF_HEAP_O))
OBJ_ESPIDF += $(addprefix $(BUILD)/, $(ESPIDF_SOC_O))
OBJ_ESPIDF += $(addprefix $(BUILD)/, $(ESPIDF_CXX_O))
OBJ_ESPIDF += $(addprefix $(BUILD)/, $(ESPIDF_ETHERNET_O))
OBJ_ESPIDF += $(addprefix $(BUILD)/, $(ESPIDF_EXPAT_O))
OBJ_ESPIDF += $(addprefix $(BUILD)/, $(ESPIDF_PTHREAD_O))
OBJ_ESPIDF += $(addprefix $(BUILD)/, $(ESPIDF_FREERTOS_O))
OBJ_ESPIDF += $(addprefix $(BUILD)/, $(ESPIDF_VFS_O))
OBJ_ESPIDF += $(addprefix $(BUILD)/, $(ESPIDF_JSON_O))
OBJ_ESPIDF += $(addprefix $(BUILD)/, $(ESPIDF_LOG_O))
OBJ_ESPIDF += $(addprefix $(BUILD)/, $(ESPIDF_LWIP_O))
OBJ_ESPIDF += $(addprefix $(BUILD)/, $(ESPIDF_MBEDTLS_O))
OBJ_ESPIDF += $(addprefix $(BUILD)/, $(ESPIDF_XTENSA_DEBUG_MODULE_O))
OBJ_ESPIDF += $(addprefix $(BUILD)/, $(ESPIDF_TCPIP_ADAPTER_O))
OBJ_ESPIDF += $(addprefix $(BUILD)/, $(ESPIDF_APP_TRACE_O))
OBJ_ESPIDF += $(addprefix $(BUILD)/, $(ESPIDF_APP_UPDATE_O))
OBJ_ESPIDF += $(addprefix $(BUILD)/, $(ESPIDF_NGHTTP_O))
OBJ_ESPIDF += $(addprefix $(BUILD)/, $(ESPIDF_NVS_FLASH_O))
OBJ_ESPIDF += $(addprefix $(BUILD)/, $(ESPIDF_OPENSSL_O))
OBJ_ESPIDF += $(addprefix $(BUILD)/, $(ESPIDF_SMARTCONFIG_ACK_O))
OBJ_ESPIDF += $(addprefix $(BUILD)/, $(ESPIDF_SPI_FLASH_O))
OBJ_ESPIDF += $(addprefix $(BUILD)/, $(ESPIDF_ULP_O))
OBJ_ESPIDF += $(addprefix $(BUILD)/, $(ESPIDF_WPA_SUPPLICANT_O))
OBJ_ESPIDF += $(addprefix $(BUILD)/, $(ESPIDF_SDMMC_O))

$(OBJ_ESPIDF): $(SDKCONFIG_H)

LIB_ESPIDF =
LIB_ESPIDF += driver
LIB_ESPIDF += efuse
LIB_ESPIDF += esp32
LIB_ESPIDF += esp_ringbuf
LIB_ESPIDF += heap
LIB_ESPIDF += soc
LIB_ESPIDF += cxx
LIB_ESPIDF += ethernet
LIB_ESPIDF += expat
LIB_ESPIDF += pthread
LIB_ESPIDF += freertos
LIB_ESPIDF += vfs
LIB_ESPIDF += json
LIB_ESPIDF += log
LIB_ESPIDF += xtensa-debug-module
LIB_ESPIDF += tcpip_adapter
LIB_ESPIDF += app_trace
LIB_ESPIDF += app_update
LIB_ESPIDF += newlib
LIB_ESPIDF += nghttp
LIB_ESPIDF += nvs_flash
LIB_ESPIDF += smartconfig_ack
LIB_ESPIDF += spi_flash
LIB_ESPIDF += ulp
LIB_ESPIDF += lwip
LIB_ESPIDF += mbedtls
LIB_ESPIDF += wpa_supplicant
LIB_ESPIDF += sdmmc

BUILD_ESPIDF_LIB = $(BUILD)/esp-idf

OBJ_ESPIDF_DIRS = $(sort $(dir $(OBJ_ESPIDF))) $(BUILD_ESPIDF_LIB) $(addprefix $(BUILD_ESPIDF_LIB)/,$(LIB_ESPIDF))
$(OBJ_ESPIDF): | $(OBJ_ESPIDF_DIRS)
$(OBJ_ESPIDF_DIRS):
	$(MKDIR) -p $@

define gen_espidf_lib_rule
$(BUILD_ESPIDF_LIB)/$(1)/lib$(1).a: $(addprefix $$(BUILD)/,$(2))
	$(ECHO) "AR $$@"
	$(Q)$(AR) cru $$@ $$^
endef

$(eval $(call gen_espidf_lib_rule,driver,$(ESPIDF_DRIVER_O)))
$(eval $(call gen_espidf_lib_rule,efuse,$(ESPIDF_EFUSE_O)))
$(eval $(call gen_espidf_lib_rule,esp32,$(ESPIDF_ESP32_O)))
$(eval $(call gen_espidf_lib_rule,esp_ringbuf,$(ESPIDF_ESP_RINGBUF_O)))
$(eval $(call gen_espidf_lib_rule,heap,$(ESPIDF_HEAP_O)))
$(eval $(call gen_espidf_lib_rule,soc,$(ESPIDF_SOC_O)))
$(eval $(call gen_espidf_lib_rule,cxx,$(ESPIDF_CXX_O)))
$(eval $(call gen_espidf_lib_rule,ethernet,$(ESPIDF_ETHERNET_O)))
$(eval $(call gen_espidf_lib_rule,expat,$(ESPIDF_EXPAT_O)))
$(eval $(call gen_espidf_lib_rule,pthread,$(ESPIDF_PTHREAD_O)))
$(eval $(call gen_espidf_lib_rule,freertos,$(ESPIDF_FREERTOS_O)))
$(eval $(call gen_espidf_lib_rule,vfs,$(ESPIDF_VFS_O)))
$(eval $(call gen_espidf_lib_rule,json,$(ESPIDF_JSON_O)))
$(eval $(call gen_espidf_lib_rule,log,$(ESPIDF_LOG_O)))
$(eval $(call gen_espidf_lib_rule,xtensa-debug-module,$(ESPIDF_XTENSA_DEBUG_MODULE_O)))
$(eval $(call gen_espidf_lib_rule,tcpip_adapter,$(ESPIDF_TCPIP_ADAPTER_O)))
$(eval $(call gen_espidf_lib_rule,app_trace,$(ESPIDF_APP_TRACE_O)))
$(eval $(call gen_espidf_lib_rule,app_update,$(ESPIDF_APP_UPDATE_O)))
$(eval $(call gen_espidf_lib_rule,newlib,$(ESPIDF_NEWLIB_O)))
$(eval $(call gen_espidf_lib_rule,nghttp,$(ESPIDF_NGHTTP_O)))
$(eval $(call gen_espidf_lib_rule,nvs_flash,$(ESPIDF_NVS_FLASH_O)))
$(eval $(call gen_espidf_lib_rule,smartconfig_ack,$(ESPIDF_SMARTCONFIG_ACK_O)))
$(eval $(call gen_espidf_lib_rule,spi_flash,$(ESPIDF_SPI_FLASH_O)))
$(eval $(call gen_espidf_lib_rule,ulp,$(ESPIDF_ULP_O)))
$(eval $(call gen_espidf_lib_rule,lwip,$(ESPIDF_LWIP_O)))
$(eval $(call gen_espidf_lib_rule,mbedtls,$(ESPIDF_MBEDTLS_O)))
$(eval $(call gen_espidf_lib_rule,wpa_supplicant,$(ESPIDF_WPA_SUPPLICANT_O)))
$(eval $(call gen_espidf_lib_rule,sdmmc,$(ESPIDF_SDMMC_O)))

LIB = $(foreach lib,$(LIB_ESPIDF),$(BUILD_ESPIDF_LIB)/$(lib)/lib$(lib).a)

################################################################################
# ESP IDF ldgen

LDGEN_FRAGMENTS = $(shell find $(ESPCOMP) -name "*.lf")
LDGEN_SECTIONS_INFO = $(foreach lib,$(LIB_ESPIDF),$(BUILD_ESPIDF_LIB)/$(lib)/lib$(lib).a.sections_info)
LDGEN_SECTION_INFOS = $(BUILD_ESPIDF_LIB)/ldgen.section_infos

define gen_sections_info_rule
$(1).sections_info: $(1)
	$(ECHO) "GEN $(1).sections_info"
	$(Q)$(OBJDUMP) -h $(1) > $(1).sections_info
endef

$(eval $(foreach lib,$(LIB_ESPIDF),$(eval $(call gen_sections_info_rule,$(BUILD_ESPIDF_LIB)/$(lib)/lib$(lib).a))))

$(LDGEN_SECTION_INFOS): $(LDGEN_SECTIONS_INFO) $(ESPIDF)/make/ldgen.mk
	$(Q)printf "$(foreach info,$(LDGEN_SECTIONS_INFO),$(info)\n)" > $@

$(BUILD)/esp32.project.ld: $(ESPCOMP)/esp32/ld/esp32.project.ld.in $(LDGEN_FRAGMENTS) $(SDKCONFIG) $(LDGEN_SECTION_INFOS)
	$(ECHO) "GEN $@"
	$(Q)$(PYTHON) $(ESPIDF)/tools/ldgen/ldgen.py \
		--input $< \
		--output $@ \
		--config $(SDKCONFIG) \
		--kconfig $(ESPIDF)/Kconfig \
		--fragments $(LDGEN_FRAGMENTS) \
		--sections $(LDGEN_SECTION_INFOS) \
		--env "IDF_TARGET=esp32" \
		--env "IDF_CMAKE=n" \
		--env "COMPONENT_KCONFIGS=$(ESPCOMP_KCONFIGS)" \
		--env "COMPONENT_KCONFIGS_PROJBUILD=$(ESPCOMP_KCONFIGS_PROJBUILD)"

################################################################################
# Main targets

all: $(BUILD)/firmware.bin

.PHONY: idf-version deploy erase

idf-version:
	$(ECHO) "ESP IDF supported hash: $(ESPIDF_SUPHASH)"

$(BUILD)/firmware.bin: $(BUILD)/bootloader.bin $(BUILD)/partitions.bin $(BUILD)/application.bin
	$(ECHO) "Create $@"
	$(Q)$(PYTHON) makeimg.py $^ $@

deploy: $(BUILD)/firmware.bin
	$(ECHO) "Writing $^ to the board"
	$(Q)$(ESPTOOL) --chip esp32 --port $(PORT) --baud $(BAUD) write_flash -z --flash_mode $(FLASH_MODE) --flash_freq $(FLASH_FREQ) 0x1000 $^

erase:
	$(ECHO) "Erasing flash"
	$(Q)$(ESPTOOL) --chip esp32 --port $(PORT) --baud $(BAUD) erase_flash

################################################################################
# Declarations to build the application

OBJ = $(OBJ_MP)

APP_LD_ARGS =
APP_LD_ARGS += $(LDFLAGS_MOD)
APP_LD_ARGS += --start-group
APP_LD_ARGS += -L$(dir $(LIBGCC_FILE_NAME)) -lgcc
APP_LD_ARGS += -L$(dir $(LIBSTDCXX_FILE_NAME)) -lstdc++
APP_LD_ARGS += $(LIBC_LIBM)
APP_LD_ARGS += $(ESPCOMP)/esp32/libhal.a
APP_LD_ARGS += -L$(ESPCOMP)/esp32/lib -lcore -lmesh -lnet80211 -lphy -lrtc -lpp -lwpa -lsmartconfig -lcoexist -lwps -lwpa2
APP_LD_ARGS += $(OBJ)
APP_LD_ARGS += $(LIB)
APP_LD_ARGS += --end-group

$(BUILD)/esp32_out.ld: $(SDKCONFIG_H)
	$(Q)$(CC) -I$(BUILD) -C -P -x c -E $(ESPCOMP)/esp32/ld/esp32.ld -o $@

$(BUILD)/application.bin: $(BUILD)/application.elf
	$(ECHO) "Create $@"
	$(Q)$(ESPTOOL) --chip esp32 elf2image --flash_mode $(FLASH_MODE) --flash_freq $(FLASH_FREQ) --flash_size $(FLASH_SIZE) $<

$(BUILD)/application.elf: $(OBJ) $(LIB) $(BUILD)/esp32_out.ld $(BUILD)/esp32.project.ld
	$(ECHO) "LINK $@"
	$(Q)$(LD) $(LDFLAGS) -o $@ $(APP_LD_ARGS)
	$(Q)$(SIZE) $@

define compile_cxx
$(ECHO) "CXX $<"
$(Q)$(CXX) $(CXXFLAGS) -c -MD -o $@ $<
@# The following fixes the dependency file.
@# See http://make.paulandlesley.org/autodep.html for details.
@# Regex adjusted from the above to play better with Windows paths, etc.
@$(CP) $(@:.o=.d) $(@:.o=.P); \
  $(SED) -e 's/#.*//' -e 's/^.*:  *//' -e 's/ *\\$$//' \
      -e '/^$$/ d' -e 's/$$/ :/' < $(@:.o=.d) >> $(@:.o=.P); \
  $(RM) -f $(@:.o=.d)
endef

vpath %.cpp . $(TOP)
$(BUILD)/%.o: %.cpp
	$(call compile_cxx)

################################################################################
# Declarations to build the bootloader

BOOTLOADER_LIB_DIR = $(BUILD)/bootloader
BOOTLOADER_LIB_ALL =

$(BUILD)/bootloader/$(ESPCOMP)/%.o: CFLAGS += -DBOOTLOADER_BUILD=1 -I$(ESPCOMP)/bootloader_support/include_priv -I$(ESPCOMP)/bootloader_support/include -I$(ESPCOMP)/micro-ecc/micro-ecc -I$(ESPCOMP)/efuse/include -I$(ESPCOMP)/esp32 -Wno-error=format

# libbootloader_support.a
BOOTLOADER_LIB_ALL += bootloader_support
BOOTLOADER_LIB_BOOTLOADER_SUPPORT_OBJ = $(addprefix $(BUILD)/bootloader/$(ESPCOMP)/,\
	bootloader_support/src/bootloader_clock.o \
	bootloader_support/src/bootloader_common.o \
	bootloader_support/src/bootloader_flash.o \
	bootloader_support/src/bootloader_init.o \
	bootloader_support/src/bootloader_random.o \
	bootloader_support/src/bootloader_sha.o \
	bootloader_support/src/bootloader_utility.o \
	bootloader_support/src/flash_qio_mode.o \
	bootloader_support/src/secure_boot_signatures.o \
	bootloader_support/src/secure_boot.o \
	bootloader_support/src/esp_image_format.o \
	bootloader_support/src/flash_encrypt.o \
	bootloader_support/src/flash_partitions.o \
	)
$(BOOTLOADER_LIB_DIR)/libbootloader_support.a: $(BOOTLOADER_LIB_BOOTLOADER_SUPPORT_OBJ)
	$(ECHO) "AR $@"
	$(Q)$(AR) cr $@ $^

# liblog.a
BOOTLOADER_LIB_ALL += log
BOOTLOADER_LIB_LOG_OBJ = $(addprefix $(BUILD)/bootloader/$(ESPCOMP)/,\
	log/log.o \
	)
$(BOOTLOADER_LIB_DIR)/liblog.a: $(BOOTLOADER_LIB_LOG_OBJ)
	$(ECHO) "AR $@"
	$(Q)$(AR) cr $@ $^

# libspi_flash.a
BOOTLOADER_LIB_ALL += spi_flash
BOOTLOADER_LIB_SPI_FLASH_OBJ = $(addprefix $(BUILD)/bootloader/$(ESPCOMP)/,\
	spi_flash/spi_flash_rom_patch.o \
	)
$(BOOTLOADER_LIB_DIR)/libspi_flash.a: $(BOOTLOADER_LIB_SPI_FLASH_OBJ)
	$(ECHO) "AR $@"
	$(Q)$(AR) cr $@ $^

# libmicro-ecc.a
BOOTLOADER_LIB_ALL += micro-ecc
BOOTLOADER_LIB_MICRO_ECC_OBJ = $(addprefix $(BUILD)/bootloader/$(ESPCOMP)/,\
	micro-ecc/micro-ecc/uECC.o \
	)
$(BOOTLOADER_LIB_DIR)/libmicro-ecc.a: $(BOOTLOADER_LIB_MICRO_ECC_OBJ)
	$(ECHO) "AR $@"
	$(Q)$(AR) cr $@ $^

# libsoc.a
$(BUILD)/bootloader/$(ESPCOMP)/soc/esp32/rtc_clk.o: CFLAGS += -fno-jump-tables -fno-tree-switch-conversion
BOOTLOADER_LIB_ALL += soc
BOOTLOADER_LIB_SOC_OBJ = $(addprefix $(BUILD)/bootloader/$(ESPCOMP)/soc/,\
	esp32/cpu_util.o \
	esp32/gpio_periph.o \
	esp32/rtc_clk.o \
	esp32/rtc_clk_init.o \
	esp32/rtc_init.o \
	esp32/rtc_periph.o \
	esp32/rtc_pm.o \
	esp32/rtc_sleep.o \
	esp32/rtc_time.o \
	esp32/rtc_wdt.o \
	esp32/sdio_slave_periph.o \
	esp32/sdmmc_periph.o \
	esp32/soc_memory_layout.o \
	esp32/spi_periph.o \
	src/memory_layout_utils.o \
	)
$(BOOTLOADER_LIB_DIR)/libsoc.a: $(BOOTLOADER_LIB_SOC_OBJ)
	$(ECHO) "AR $@"
	$(Q)$(AR) cr $@ $^

# libmain.a
BOOTLOADER_LIB_ALL += main
BOOTLOADER_LIB_MAIN_OBJ = $(addprefix $(BUILD)/bootloader/$(ESPCOMP)/,\
	bootloader/subproject/main/bootloader_start.o \
	)
$(BOOTLOADER_LIB_DIR)/libmain.a: $(BOOTLOADER_LIB_MAIN_OBJ)
	$(ECHO) "AR $@"
	$(Q)$(AR) cr $@ $^

# all objects files
BOOTLOADER_OBJ_ALL = \
	$(BOOTLOADER_LIB_BOOTLOADER_SUPPORT_OBJ) \
	$(BOOTLOADER_LIB_LOG_OBJ) \
	$(BOOTLOADER_LIB_SPI_FLASH_OBJ) \
	$(BOOTLOADER_LIB_MICRO_ECC_OBJ) \
	$(BOOTLOADER_LIB_SOC_OBJ) \
	$(BOOTLOADER_LIB_MAIN_OBJ)

$(BOOTLOADER_OBJ_ALL): $(SDKCONFIG_H)

BOOTLOADER_LIBS =
BOOTLOADER_LIBS += -Wl,--start-group
BOOTLOADER_LIBS += $(BOOTLOADER_OBJ)
BOOTLOADER_LIBS += -L$(BUILD)/bootloader $(addprefix -l,$(BOOTLOADER_LIB_ALL))
BOOTLOADER_LIBS += -L$(ESPCOMP)/esp32/lib -lrtc
BOOTLOADER_LIBS += -L$(dir $(LIBGCC_FILE_NAME)) -lgcc
BOOTLOADER_LIBS += -Wl,--end-group

BOOTLOADER_LDFLAGS =
BOOTLOADER_LDFLAGS += -nostdlib
BOOTLOADER_LDFLAGS += -L$(ESPIDF)/lib
BOOTLOADER_LDFLAGS += -L$(ESPIDF)/ld
BOOTLOADER_LDFLAGS += -u call_user_start_cpu0
BOOTLOADER_LDFLAGS += -Wl,--gc-sections
BOOTLOADER_LDFLAGS += -static
BOOTLOADER_LDFLAGS += -Wl,-EL
BOOTLOADER_LDFLAGS += -Wl,-Map=$(@:.elf=.map) -Wl,--cref
BOOTLOADER_LDFLAGS += -T $(ESPCOMP)/bootloader/subproject/main/esp32.bootloader.ld
BOOTLOADER_LDFLAGS += -T $(ESPCOMP)/bootloader/subproject/main/esp32.bootloader.rom.ld
BOOTLOADER_LDFLAGS += -T $(ESPCOMP)/esp32/ld/esp32.rom.ld
BOOTLOADER_LDFLAGS += -T $(ESPCOMP)/esp32/ld/esp32.rom.spiram_incompatible_fns.ld
BOOTLOADER_LDFLAGS += -T $(ESPCOMP)/esp32/ld/esp32.peripherals.ld

BOOTLOADER_OBJ_DIRS = $(sort $(dir $(BOOTLOADER_OBJ_ALL)))
$(BOOTLOADER_OBJ_ALL): | $(BOOTLOADER_OBJ_DIRS)
$(BOOTLOADER_OBJ_DIRS):
	$(MKDIR) -p $@

$(BUILD)/bootloader/%.o: %.c
	$(call compile_c)

$(BUILD)/bootloader.bin: $(BUILD)/bootloader.elf
	$(ECHO) "Create $@"
	$(Q)$(ESPTOOL) --chip esp32 elf2image --flash_mode $(FLASH_MODE) --flash_freq $(FLASH_FREQ) --flash_size $(FLASH_SIZE) $<

$(BUILD)/bootloader.elf: $(BOOTLOADER_OBJ) $(addprefix $(BOOTLOADER_LIB_DIR)/lib,$(addsuffix .a,$(BOOTLOADER_LIB_ALL)))
	$(ECHO) "LINK $@"
	$(Q)$(CC) $(BOOTLOADER_LDFLAGS) -o $@ $(BOOTLOADER_LIBS)

################################################################################
# Declarations to build the partitions

PYTHON2 ?= python2
PART_SRC = partitions.csv

$(BUILD)/partitions.bin: $(PART_SRC)
	$(ECHO) "Create $@"
	$(Q)$(PYTHON2) $(ESPCOMP)/partition_table/gen_esp32part.py -q $< $@

################################################################################

include $(TOP)/py/mkrules.mk

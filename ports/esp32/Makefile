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

# paths to ESP IDF and its components
ifeq ($(ESPIDF),)
$(error Please configure the ESPIDF variable)
endif
ESPCOMP = $(ESPIDF)/components
ESPTOOL ?= $(ESPCOMP)/esptool_py/esptool/esptool.py

# verify the ESP IDF version
ESPIDF_SUPHASH := 9a26296a0e88a4c3ae27e9c848be970946fff87e
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

INC += -I.
INC += -I$(TOP)
INC += -I$(TOP)/lib/mp-readline
INC += -I$(TOP)/lib/netutils
INC += -I$(TOP)/lib/timeutils
INC += -I$(BUILD)

INC_ESPCOMP += -I$(ESPCOMP)/bootloader_support/include
INC_ESPCOMP += -I$(ESPCOMP)/driver/include
INC_ESPCOMP += -I$(ESPCOMP)/driver/include/driver
INC_ESPCOMP += -I$(ESPCOMP)/nghttp/port/include
INC_ESPCOMP += -I$(ESPCOMP)/nghttp/nghttp2/lib/includes
INC_ESPCOMP += -I$(ESPCOMP)/esp32/include
INC_ESPCOMP += -I$(ESPCOMP)/soc/include
INC_ESPCOMP += -I$(ESPCOMP)/soc/esp32/include
INC_ESPCOMP += -I$(ESPCOMP)/ethernet/include
INC_ESPCOMP += -I$(ESPCOMP)/expat/include/expat
INC_ESPCOMP += -I$(ESPCOMP)/expat/port/include
INC_ESPCOMP += -I$(ESPCOMP)/heap/include
INC_ESPCOMP += -I$(ESPCOMP)/json/include
INC_ESPCOMP += -I$(ESPCOMP)/json/port/include
INC_ESPCOMP += -I$(ESPCOMP)/log/include
INC_ESPCOMP += -I$(ESPCOMP)/newlib/include
INC_ESPCOMP += -I$(ESPCOMP)/nvs_flash/include
INC_ESPCOMP += -I$(ESPCOMP)/freertos/include
INC_ESPCOMP += -I$(ESPCOMP)/tcpip_adapter/include
INC_ESPCOMP += -I$(ESPCOMP)/lwip/include/lwip
INC_ESPCOMP += -I$(ESPCOMP)/lwip/include/lwip/port
INC_ESPCOMP += -I$(ESPCOMP)/lwip/include/lwip/posix
INC_ESPCOMP += -I$(ESPCOMP)/mbedtls/include
INC_ESPCOMP += -I$(ESPCOMP)/mbedtls/port/include
INC_ESPCOMP += -I$(ESPCOMP)/spi_flash/include
INC_ESPCOMP += -I$(ESPCOMP)/vfs/include
INC_ESPCOMP += -I$(ESPCOMP)/newlib/platform_include
INC_ESPCOMP += -I$(ESPCOMP)/xtensa-debug-module/include
INC_ESPCOMP += -I$(ESPCOMP)/wpa_supplicant/include
INC_ESPCOMP += -I$(ESPCOMP)/wpa_supplicant/port/include
INC_ESPCOMP += -I$(ESPCOMP)/ethernet/include
INC_ESPCOMP += -I$(ESPCOMP)/app_trace/include

CFLAGS_BASE = -std=gnu99 -Os -ffunction-sections -fdata-sections -fstrict-volatile-bitfields -mlongcalls -nostdlib -Wall -Werror -Wno-error=unused-function -Wno-error=unused-but-set-variable -Wno-error=unused-variable -Wno-error=deprecated-declarations -DMBEDTLS_CONFIG_FILE='"mbedtls/esp_config.h"' -DHAVE_CONFIG_H -DESP_PLATFORM
CFLAGS = $(CFLAGS_BASE) $(INC) $(INC_ESPCOMP)
CFLAGS += -DIDF_VER=\"$(IDF_VER)\"
CFLAGS += $(CFLAGS_MOD)

# this is what ESPIDF uses for c++ compilation
CXXFLAGS = -std=gnu++11 -fno-exceptions -fno-rtti -ffunction-sections -fdata-sections -fstrict-volatile-bitfields -mlongcalls -nostdlib -Wall -Werror -Wno-error=unused-function -Wno-error=unused-but-set-variable -Wno-error=unused-variable -Wno-error=deprecated-declarations -DESP_PLATFORM $(INC) $(INC_ESPCOMP)

LDFLAGS = -nostdlib -Map=$(@:.elf=.map) --cref
LDFLAGS += --gc-sections -static -EL
LDFLAGS += -u call_user_start_cpu0 -u uxTopUsedPriority -u ld_include_panic_highint_hdl
LDFLAGS += -u __cxa_guard_dummy # so that implementation of static guards is taken from cxx_guards.o instead of libstdc++.a
LDFLAGS += -L$(ESPCOMP)/esp32/ld
LDFLAGS += -T $(BUILD)/esp32_out.ld
LDFLAGS += -T ./esp32.custom_common.ld
LDFLAGS += -T esp32.rom.ld
LDFLAGS += -T esp32.rom.spiram_incompatible_fns.ld
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
	modsocket.c \
	modesp.c \
	moduhashlib.c \
	espneopixel.c \
	machine_hw_spi.c \
	mpthreadport.c \
	$(SRC_MOD)

EXTMOD_SRC_C = $(addprefix extmod/,\
	modonewire.c \
	)

LIB_SRC_C = $(addprefix lib/,\
	libm/math.c \
	libm/fmodf.c \
	libm/roundf.c \
	libm/ef_sqrt.c \
	libm/kf_rem_pio2.c \
	libm/kf_sin.c \
	libm/kf_cos.c \
	libm/kf_tan.c \
	libm/ef_rem_pio2.c \
	libm/sf_sin.c \
	libm/sf_cos.c \
	libm/sf_tan.c \
	libm/sf_frexp.c \
	libm/sf_modf.c \
	libm/sf_ldexp.c \
	libm/asinfacosf.c \
	libm/atanf.c \
	libm/atan2f.c \
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
	lib/oofatfs/option/unicode.c
endif

DRIVERS_SRC_C = $(addprefix drivers/,\
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
# List of object files from the ESP32 IDF components

ESPIDF_DRIVER_O = $(addprefix $(ESPCOMP)/driver/,\
	uart.o \
	periph_ctrl.o \
	ledc.o \
	gpio.o \
	timer.o \
	spi_master.o \
	spi_common.o \
	rtc_module.o \
	)

$(BUILD)/$(ESPCOMP)/esp32/dport_access.o: CFLAGS += -Wno-array-bounds
ESPIDF_ESP32_O = $(addprefix $(ESPCOMP)/esp32/,\
	panic.o \
	esp_timer.o \
	esp_timer_esp32.o \
	ets_timer_legacy.o \
	event_default_handlers.o \
	fast_crypto_ops.o \
	task_wdt.o \
	cache_err_int.o \
	clk.o \
	core_dump.o \
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
	intr_alloc.o \
	dport_access.o \
	wifi_init.o \
	)

ESPIDF_HEAP_O = $(addprefix $(ESPCOMP)/heap/,\
	heap_caps.o \
	heap_caps_init.o \
	multi_heap.o \
	)

ESPIDF_SOC_O = $(addprefix $(ESPCOMP)/soc/,\
	esp32/cpu_util.o \
	esp32/rtc_clk.o \
	esp32/rtc_init.o \
	esp32/rtc_pm.o \
	esp32/rtc_sleep.o \
	esp32/rtc_time.o \
	esp32/soc_memory_layout.o \
	)

ESPIDF_CXX_O = $(addprefix $(ESPCOMP)/cxx/,\
	cxx_guards.o \
	)

ESPIDF_ETHERNET_O = $(addprefix $(ESPCOMP)/ethernet/,\
	emac_dev.o \
	emac_main.o \
	)

$(BUILD)/$(ESPCOMP)/expat/%.o: CFLAGS += -Wno-unused-function
ESPIDF_EXPAT_O = $(addprefix $(ESPCOMP)/expat/,\
	library/xmltok_ns.o \
	library/xmltok.o \
	library/xmlparse.o \
	library/xmlrole.o \
	library/xmltok_impl.o \
	port/minicheck.o \
	port/expat_element.o \
	port/chardata.o \
	)

ESPIDF_PTHREAD_O = $(addprefix $(ESPCOMP)/pthread/,\
	pthread.o \
	)

# Assembler .S files need only basic flags, and in particular should not have
# -Os because that generates subtly different code.
# We also need custom CFLAGS for .c files because FreeRTOS has headers with
# generic names (eg queue.h) which can clash with other files in the port.
CFLAGS_ASM = -I$(ESPCOMP)/esp32/include -I$(ESPCOMP)/soc/esp32/include -I$(ESPCOMP)/freertos/include/freertos -I.
$(BUILD)/$(ESPCOMP)/freertos/portasm.o: CFLAGS = $(CFLAGS_ASM)
$(BUILD)/$(ESPCOMP)/freertos/xtensa_context.o: CFLAGS = $(CFLAGS_ASM)
$(BUILD)/$(ESPCOMP)/freertos/xtensa_intr_asm.o: CFLAGS = $(CFLAGS_ASM)
$(BUILD)/$(ESPCOMP)/freertos/xtensa_vectors.o: CFLAGS = $(CFLAGS_ASM)
$(BUILD)/$(ESPCOMP)/freertos/%.o: CFLAGS = $(CFLAGS_BASE) -I. $(INC_ESPCOMP) -I$(ESPCOMP)/freertos/include/freertos
ESPIDF_FREERTOS_O = $(addprefix $(ESPCOMP)/freertos/,\
	croutine.o \
	event_groups.o \
	FreeRTOS-openocd.o \
	list.o \
	portasm.o \
	port.o \
	queue.o \
	ringbuf.o \
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

ESPIDF_JSON_O = $(addprefix $(ESPCOMP)/json/,\
	library/cJSON.o \
	port/cJSON_Utils.o \
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

ESPIDF_NEWLIB_O = $(addprefix $(ESPCOMP)/newlib/,\
	time.o \
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
	src/nvs_types.o \
	src/nvs_page.o \
	src/nvs_item_hash_list.o \
	src/nvs_pagemanager.o \
	src/nvs_storage.o \
	src/nvs_api.o \
	)

ESPIDF_OPENSSL_O = $(addprefix $(ESPCOMP)/openssl/,\
	)

ESPIDF_SPI_FLASH_O = $(addprefix $(ESPCOMP)/spi_flash/,\
	flash_mmap.o \
	partition.o \
	spi_flash_rom_patch.o \
	cache_utils.o \
	flash_ops.o \
	)

$(BUILD)/$(ESPCOMP)/lwip/%.o: CFLAGS += -Wno-address -Wno-unused-variable -Wno-unused-but-set-variable
ESPIDF_LWIP_O = $(addprefix $(ESPCOMP)/lwip/,\
	api/pppapi.o \
	api/netbuf.o \
	api/api_lib.o \
	api/netifapi.o \
	api/tcpip.o \
	api/netdb.o \
	api/err.o \
	api/api_msg.o \
	api/sockets.o \
	apps/sntp/sntp.o \
	apps/dhcpserver.o \
	core/ipv4/ip_frag.o \
	core/ipv4/dhcp.o \
	core/ipv4/ip4_addr.o \
	core/ipv4/igmp.o \
	core/ipv4/ip4.o \
	core/ipv4/autoip.o \
	core/ipv4/icmp.o \
	core/ipv6/ip6_frag.o \
	core/ipv6/dhcp6.o \
	core/ipv6/inet6.o \
	core/ipv6/ip6_addr.o \
	core/ipv6/ip6.o \
	core/ipv6/nd6.o \
	core/ipv6/mld6.o \
	core/ipv6/ethip6.o \
	core/ipv6/icmp6.o \
	core/mem.o \
	core/init.o \
	core/memp.o \
	core/sys.o \
	core/tcp_in.o \
	core/dns.o \
	core/ip.o \
	core/pbuf.o \
	core/raw.o \
	core/tcp.o \
	core/def.o \
	core/netif.o \
	core/stats.o \
	core/timers.o \
	core/inet_chksum.o \
	core/udp.o \
	core/tcp_out.o \
	netif/slipif.o \
	netif/etharp.o \
	netif/ethernet.o \
	netif/lowpan6.o \
	netif/ethernetif.o \
	port/freertos/sys_arch.o \
	port/netif/wlanif.o \
	port/netif/ethernetif.o \
	)

ESPIDF_MBEDTLS_O = $(addprefix $(ESPCOMP)/mbedtls/,\
	library/entropy.o \
	library/pkcs12.o \
	library/ccm.o \
	library/pk.o \
	library/sha1.o \
	library/x509_csr.o \
	library/ssl_cli.o \
	library/ecp.o \
	library/blowfish.o \
	library/x509.o \
	library/ecp_curves.o \
	library/error.o \
	library/ssl_ticket.o \
	library/entropy_poll.o \
	library/cipher.o \
	library/version_features.o \
	library/ripemd160.o \
	library/rsa.o \
	library/md.o \
	library/md_wrap.o \
	library/sha256.o \
	library/dhm.o \
	library/ssl_cache.o \
	library/pkwrite.o \
	library/base64.o \
	library/asn1parse.o \
	library/ssl_tls.o \
	library/hmac_drbg.o \
	library/pem.o \
	library/version.o \
	library/gcm.o \
	library/memory_buffer_alloc.o \
	library/md2.o \
	library/ecdsa.o \
	library/ssl_srv.o \
	library/x509_crt.o \
	library/ecdh.o \
	library/asn1write.o \
	library/md4.o \
	library/debug.o \
	library/x509_create.o \
	library/ecjpake.o \
	library/oid.o \
	library/md5.o \
	library/ssl_ciphersuites.o \
	library/sha512.o \
	library/xtea.o \
	library/aes.o \
	library/cipher_wrap.o \
	library/arc4.o \
	library/bignum.o \
	library/pkparse.o \
	library/padlock.o \
	library/threading.o \
	library/x509_crl.o \
	library/pkcs11.o \
	library/aesni.o \
	library/timing.o \
	library/certs.o \
	library/pkcs5.o \
	library/ssl_cookie.o \
	library/camellia.o \
	library/havege.o \
	library/des.o \
	library/x509write_csr.o \
	library/platform.o \
	library/ctr_drbg.o \
	library/x509write_crt.o \
	library/pk_wrap.o \
	port/esp_bignum.o \
	port/esp_hardware.o \
	port/esp_sha1.o \
	port/esp_sha256.o \
	port/esp_sha512.o \
	)

$(BUILD)/$(ESPCOMP)/wpa_supplicant/%.o: CFLAGS += -DEMBEDDED_SUPP -D__ets__ -Wno-strict-aliasing
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

OBJ_ESPIDF =
OBJ_ESPIDF += $(addprefix $(BUILD)/, $(ESPIDF_NEWLIB_O))
OBJ_ESPIDF += $(addprefix $(BUILD)/, $(ESPIDF_DRIVER_O))
OBJ_ESPIDF += $(addprefix $(BUILD)/, $(ESPIDF_ESP32_O))
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
OBJ_ESPIDF += $(addprefix $(BUILD)/, $(ESPIDF_NGHTTP_O))
OBJ_ESPIDF += $(addprefix $(BUILD)/, $(ESPIDF_NVS_FLASH_O))
OBJ_ESPIDF += $(addprefix $(BUILD)/, $(ESPIDF_OPENSSL_O))
OBJ_ESPIDF += $(addprefix $(BUILD)/, $(ESPIDF_SPI_FLASH_O))
OBJ_ESPIDF += $(addprefix $(BUILD)/, $(ESPIDF_WPA_SUPPLICANT_O))
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

OBJ = $(OBJ_MP) $(OBJ_ESPIDF)

APP_LD_ARGS =
APP_LD_ARGS += $(LDFLAGS_MOD)
APP_LD_ARGS += --start-group
APP_LD_ARGS += -L$(dir $(LIBGCC_FILE_NAME)) -lgcc
APP_LD_ARGS += -L$(dir $(LIBSTDCXX_FILE_NAME)) -lstdc++
APP_LD_ARGS += $(ESPCOMP)/newlib/lib/libc.a
APP_LD_ARGS += $(ESPCOMP)/newlib/lib/libm.a
APP_LD_ARGS += $(ESPCOMP)/esp32/libhal.a
APP_LD_ARGS += -L$(ESPCOMP)/esp32/lib -lcore -lnet80211 -lphy -lrtc -lpp -lwpa -lsmartconfig -lcoexist
APP_LD_ARGS += $(OBJ)
APP_LD_ARGS += --end-group

$(BUILD)/esp32_out.ld: sdkconfig.h
	$(Q)$(CC) -I. -C -P -x c -E $(ESPCOMP)/esp32/ld/esp32.ld -o $@

$(BUILD)/application.bin: $(BUILD)/application.elf
	$(ECHO) "Create $@"
	$(Q)$(ESPTOOL) --chip esp32 elf2image --flash_mode $(FLASH_MODE) --flash_freq $(FLASH_FREQ) --flash_size $(FLASH_SIZE) $<

$(BUILD)/application.elf: $(OBJ) $(BUILD)/esp32_out.ld
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

$(BUILD)/bootloader/$(ESPCOMP)/%.o: CFLAGS += -DBOOTLOADER_BUILD=1 -I$(ESPCOMP)/bootloader_support/include_priv -I$(ESPCOMP)/bootloader_support/include -I$(ESPCOMP)/micro-ecc/micro-ecc -I$(ESPCOMP)/esp32 -Wno-error=format
BOOTLOADER_OBJ = $(addprefix $(BUILD)/bootloader/$(ESPCOMP)/,\
	bootloader_support/src/bootloader_flash.o \
	bootloader_support/src/bootloader_random.o \
	bootloader_support/src/bootloader_sha.o \
	bootloader_support/src/secure_boot_signatures.o \
	bootloader_support/src/secure_boot.o \
	bootloader_support/src/esp_image_format.o \
	bootloader_support/src/flash_encrypt.o \
	bootloader_support/src/flash_partitions.o \
	log/log.o \
	spi_flash/spi_flash_rom_patch.o \
	soc/esp32/rtc_clk.o \
	soc/esp32/rtc_time.o \
	micro-ecc/micro-ecc/uECC.o \
	bootloader/subproject/main/bootloader_start.o \
	)

BOOTLOADER_LIBS =
BOOTLOADER_LIBS += -Wl,--start-group
BOOTLOADER_LIBS += $(BOOTLOADER_OBJ)
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

BOOTLOADER_OBJ_DIRS = $(sort $(dir $(BOOTLOADER_OBJ)))
$(BOOTLOADER_OBJ): | $(BOOTLOADER_OBJ_DIRS)
$(BOOTLOADER_OBJ_DIRS):
	$(MKDIR) -p $@

$(BUILD)/bootloader/%.o: %.c
	$(call compile_c)

$(BUILD)/bootloader.bin: $(BUILD)/bootloader.elf
	$(ECHO) "Create $@"
	$(Q)$(ESPTOOL) --chip esp32 elf2image --flash_mode $(FLASH_MODE) --flash_freq $(FLASH_FREQ) --flash_size $(FLASH_SIZE) $<

$(BUILD)/bootloader.elf: $(BOOTLOADER_OBJ)
	$(ECHO) "LINK $@"
	$(Q)$(CC) $(BOOTLOADER_LDFLAGS) -o $@ $(BOOTLOADER_LIBS)

################################################################################
# Declarations to build the partitions

PYTHON2 ?= python2
PART_SRC = $(ESPCOMP)/partition_table/partitions_singleapp.csv

$(BUILD)/partitions.bin: $(PART_SRC)
	$(ECHO) "Create $@"
	$(Q)$(PYTHON2) $(ESPCOMP)/partition_table/gen_esp32part.py -q $< $@

################################################################################

include $(TOP)/py/mkrules.mk

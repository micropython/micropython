# This makefile fragment adds the source code files for the core extmod modules
# and provides rules to build 3rd-party components for extmod modules.

SRC_EXTMOD_C += \
	extmod/machine_adc.c \
	extmod/machine_adc_block.c \
	extmod/machine_bitstream.c \
	extmod/machine_i2c.c \
	extmod/machine_i2s.c \
	extmod/machine_mem.c \
	extmod/machine_pinbase.c \
	extmod/machine_pulse.c \
	extmod/machine_pwm.c \
	extmod/machine_signal.c \
	extmod/machine_spi.c \
	extmod/machine_timer.c \
	extmod/machine_uart.c \
	extmod/machine_usb_device.c \
	extmod/machine_wdt.c \
	extmod/modasyncio.c \
	extmod/modbinascii.c \
	extmod/modbluetooth.c \
	extmod/modbtree.c \
	extmod/modcryptolib.c \
	extmod/moddeflate.c \
	extmod/modframebuf.c \
	extmod/modhashlib.c \
	extmod/modheapq.c \
	extmod/modjson.c \
	extmod/modlwip.c \
	extmod/modmachine.c \
	extmod/modmarshal.c \
	extmod/modnetwork.c \
	extmod/modonewire.c \
	extmod/modopenamp.c \
	extmod/modopenamp_remoteproc.c \
	extmod/modopenamp_remoteproc_store.c \
	extmod/modos.c \
	extmod/modplatform.c\
	extmod/modrandom.c \
	extmod/modre.c \
	extmod/modselect.c \
	extmod/modsocket.c \
	extmod/modtls_axtls.c \
	extmod/modtls_mbedtls.c \
	extmod/mbedtls/mbedtls_alt.c \
	extmod/modtime.c \
	extmod/moductypes.c \
	extmod/modvfs.c \
	extmod/modwebrepl.c \
	extmod/modwebsocket.c \
	extmod/network_cyw43.c \
	extmod/network_esp_hosted.c \
	extmod/network_lwip.c \
	extmod/network_ninaw10.c \
	extmod/network_ppp_lwip.c \
	extmod/network_wiznet5k.c \
	extmod/os_dupterm.c \
	extmod/vfs.c \
	extmod/vfs_blockdev.c \
	extmod/vfs_fat.c \
	extmod/vfs_fat_diskio.c \
	extmod/vfs_fat_file.c \
	extmod/vfs_lfs.c \
	extmod/vfs_rom.c \
	extmod/vfs_rom_file.c \
	extmod/vfs_posix.c \
	extmod/vfs_posix_file.c \
	extmod/vfs_reader.c \
	extmod/virtpin.c \
	shared/libc/abort_.c \
	shared/libc/printf.c \

SRC_THIRDPARTY_C += \

PY_O += $(addprefix $(BUILD)/, $(SRC_EXTMOD_C:.c=.o))
PY_O += $(addprefix $(BUILD)/, $(SRC_THIRDPARTY_C:.c=.o))
SRC_QSTR += $(SRC_EXTMOD_C)

CFLAGS += $(CFLAGS_EXTMOD) $(CFLAGS_THIRDPARTY)
LDFLAGS += $(LDFLAGS_EXTMOD) $(LDFLAGS_THIRDPARTY)

################################################################################
# libm/libm_dbl math library

# Single-precision math library.
SRC_LIB_LIBM_C += $(addprefix lib/libm/,\
	acoshf.c \
	asinfacosf.c \
	asinhf.c \
	atan2f.c \
	atanf.c \
	atanhf.c \
	ef_rem_pio2.c \
	erf_lgamma.c \
	fmodf.c \
	kf_cos.c \
	kf_rem_pio2.c \
	kf_sin.c \
	kf_tan.c \
	log1pf.c \
	math.c \
	nearbyintf.c \
	roundf.c \
	sf_cos.c \
	sf_erf.c \
	sf_frexp.c \
	sf_ldexp.c \
	sf_modf.c \
	sf_sin.c \
	sf_tan.c \
	wf_lgamma.c \
	wf_tgamma.c \
	)

# Choose only one of these sqrt implementations, software or hardware.
SRC_LIB_LIBM_SQRT_SW_C += lib/libm/ef_sqrt.c
SRC_LIB_LIBM_SQRT_HW_C += lib/libm/thumb_vfp_sqrtf.c

# Disable warnings in libm.
$(BUILD)/lib/libm/kf_rem_pio2.o: CFLAGS += -Wno-maybe-uninitialized

# Double-precision math library.
SRC_LIB_LIBM_DBL_C += $(addprefix lib/libm_dbl/,\
	__cos.c \
	__expo2.c \
	__fpclassify.c \
	__rem_pio2.c \
	__rem_pio2_large.c \
	__signbit.c \
	__sin.c \
	__tan.c \
	acos.c \
	acosh.c \
	asin.c \
	asinh.c \
	atan.c \
	atan2.c \
	atanh.c \
	ceil.c \
	cos.c \
	cosh.c \
	copysign.c \
	erf.c \
	exp.c \
	expm1.c \
	floor.c \
	fmod.c \
	frexp.c \
	ldexp.c \
	lgamma.c \
	log.c \
	log10.c \
	log1p.c \
	modf.c \
	nearbyint.c \
	pow.c \
	rint.c \
	round.c \
	scalbn.c \
	sin.c \
	sinh.c \
	tan.c \
	tanh.c \
	tgamma.c \
	trunc.c \
	)

# Choose only one of these sqrt implementations, software or hardware.
SRC_LIB_LIBM_DBL_SQRT_SW_C += lib/libm_dbl/sqrt.c
SRC_LIB_LIBM_DBL_SQRT_HW_C += lib/libm_dbl/thumb_vfp_sqrt.c

# Too many warnings in libm_dbl, disable for now.
$(BUILD)/lib/libm_dbl/%.o: CFLAGS += -Wno-double-promotion -Wno-float-conversion
$(BUILD)/lib/libm_dbl/__rem_pio2_large.o: CFLAGS += -Wno-maybe-uninitialized

################################################################################
# VFS FAT FS

OOFATFS_DIR = lib/oofatfs

# this sets the config file for FatFs
CFLAGS_THIRDPARTY += -DFFCONF_H=\"$(OOFATFS_DIR)/ffconf.h\"

ifeq ($(MICROPY_VFS_FAT),1)
CFLAGS_EXTMOD += -DMICROPY_VFS_FAT=1
SRC_THIRDPARTY_C += $(addprefix $(OOFATFS_DIR)/,\
	ff.c \
	ffunicode.c \
	)
endif

################################################################################
# VFS littlefs

LITTLEFS_DIR = lib/littlefs

ifeq ($(MICROPY_VFS_LFS1),1)
CFLAGS_EXTMOD += -DMICROPY_VFS_LFS1=1
CFLAGS_THIRDPARTY += -DLFS1_NO_MALLOC -DLFS1_NO_DEBUG -DLFS1_NO_WARN -DLFS1_NO_ERROR -DLFS1_NO_ASSERT
SRC_THIRDPARTY_C += $(addprefix $(LITTLEFS_DIR)/,\
	lfs1.c \
	lfs1_util.c \
	)
endif

ifeq ($(MICROPY_VFS_LFS2),1)
CFLAGS_EXTMOD += -DMICROPY_VFS_LFS2=1
CFLAGS_THIRDPARTY += -DLFS2_NO_MALLOC -DLFS2_NO_DEBUG -DLFS2_NO_WARN -DLFS2_NO_ERROR -DLFS2_NO_ASSERT -DLFS2_DEFINES=extmod/littlefs-include/lfs2_defines.h
SRC_THIRDPARTY_C += $(addprefix $(LITTLEFS_DIR)/,\
	lfs2.c \
	lfs2_util.c \
	)

$(BUILD)/$(LITTLEFS_DIR)/lfs2.o: CFLAGS += -Wno-shadow
endif

################################################################################
# ussl

ifeq ($(MICROPY_PY_SSL),1)
CFLAGS_EXTMOD += -DMICROPY_PY_SSL=1
ifeq ($(MICROPY_SSL_AXTLS),1)
AXTLS_DIR = lib/axtls
GIT_SUBMODULES += $(AXTLS_DIR)
CFLAGS_EXTMOD += -DMICROPY_SSL_AXTLS=1 -I$(TOP)/lib/axtls/ssl -I$(TOP)/lib/axtls/crypto -I$(TOP)/extmod/axtls-include
$(BUILD)/$(AXTLS_DIR)/%.o: CFLAGS += -Wno-all -Wno-unused-parameter -Wno-uninitialized -Wno-sign-compare -Wno-old-style-definition -Dmp_stream_errno=errno $(AXTLS_DEFS_EXTRA)
SRC_THIRDPARTY_C += $(addprefix $(AXTLS_DIR)/,\
	ssl/asn1.c \
	ssl/loader.c \
	ssl/tls1.c \
	ssl/tls1_svr.c \
	ssl/tls1_clnt.c \
	ssl/x509.c \
	crypto/aes.c \
	crypto/bigint.c \
	crypto/crypto_misc.c \
	crypto/hmac.c \
	crypto/md5.c \
	crypto/rsa.c \
	crypto/sha1.c \
	)
else ifeq ($(MICROPY_SSL_MBEDTLS),1)
MBEDTLS_DIR = lib/mbedtls
MBEDTLS_CONFIG_FILE ?= \"mbedtls/mbedtls_config_port.h\"
GIT_SUBMODULES += $(MBEDTLS_DIR)
CFLAGS_EXTMOD += -DMBEDTLS_CONFIG_FILE=$(MBEDTLS_CONFIG_FILE)
CFLAGS_EXTMOD += -DMICROPY_SSL_MBEDTLS=1 -I$(TOP)/$(MBEDTLS_DIR)/include
ifeq ($(MICROPY_PY_SSL_ECDSA_SIGN_ALT),1)
CFLAGS_EXTMOD += -DMICROPY_PY_SSL_ECDSA_SIGN_ALT=1
LDFLAGS_EXTMOD += -Wl,--wrap=mbedtls_ecdsa_write_signature
endif
SRC_THIRDPARTY_C += lib/mbedtls_errors/mp_mbedtls_errors.c
SRC_THIRDPARTY_C += $(addprefix $(MBEDTLS_DIR)/library/,\
	aes.c \
	aesni.c \
	asn1parse.c \
	asn1write.c \
	base64.c \
	bignum_core.c \
	bignum_mod.c \
	bignum_mod_raw.c \
	bignum.c \
	camellia.c \
	ccm.c \
	chacha20.c \
	chachapoly.c \
	cipher.c \
	cipher_wrap.c \
	nist_kw.c \
	aria.c \
	cmac.c \
	constant_time.c \
	mps_reader.c \
	mps_trace.c \
	ctr_drbg.c \
	debug.c \
	des.c \
	dhm.c \
	ecdh.c \
	ecdsa.c \
	ecjpake.c \
	ecp.c \
	ecp_curves.c \
	entropy.c \
	entropy_poll.c \
	gcm.c \
	hmac_drbg.c \
	md5.c \
	md.c \
	oid.c \
	padlock.c \
	pem.c \
	pk.c \
	pkcs12.c \
	pkcs5.c \
	pkparse.c \
	pk_ecc.c \
	pk_wrap.c \
	pkwrite.c \
	platform.c \
	platform_util.c \
	poly1305.c \
	ripemd160.c \
	rsa.c \
	rsa_alt_helpers.c \
	sha1.c \
	sha256.c \
	sha512.c \
	ssl_cache.c \
	ssl_ciphersuites.c \
	ssl_client.c \
	ssl_cookie.c \
	ssl_debug_helpers_generated.c \
	ssl_msg.c \
	ssl_ticket.c \
	ssl_tls.c \
	ssl_tls12_client.c \
	ssl_tls12_server.c \
	timing.c \
	x509.c \
	x509_create.c \
	x509_crl.c \
	x509_crt.c \
	x509_csr.c \
	x509write_crt.c \
	x509write_csr.c \
	)
endif
endif

################################################################################
# lwip

ifeq ($(MICROPY_PY_LWIP),1)
GIT_SUBMODULES += lib/lwip
# A port should add an include path where lwipopts.h can be found (eg extmod/lwip-include)
LWIP_DIR = lib/lwip/src
INC += -I$(TOP)/$(LWIP_DIR)/include
CFLAGS_EXTMOD += -DMICROPY_PY_LWIP=1
$(BUILD)/$(LWIP_DIR)/core/ipv4/dhcp.o: CFLAGS += -Wno-address
SRC_THIRDPARTY_C += shared/netutils/netutils.c
SRC_THIRDPARTY_C += $(addprefix $(LWIP_DIR)/,\
	apps/mdns/mdns.c \
	apps/mdns/mdns_domain.c \
	apps/mdns/mdns_out.c \
	core/def.c \
	core/dns.c \
	core/inet_chksum.c \
	core/init.c \
	core/ip.c \
	core/mem.c \
	core/memp.c \
	core/netif.c \
	core/pbuf.c \
	core/raw.c \
	core/stats.c \
	core/sys.c \
	core/tcp.c \
	core/tcp_in.c \
	core/tcp_out.c \
	core/timeouts.c \
	core/udp.c \
	core/ipv4/acd.c \
	core/ipv4/autoip.c \
	core/ipv4/dhcp.c \
	core/ipv4/etharp.c \
	core/ipv4/icmp.c \
	core/ipv4/igmp.c \
	core/ipv4/ip4_addr.c \
	core/ipv4/ip4.c \
	core/ipv4/ip4_frag.c \
	core/ipv6/dhcp6.c \
	core/ipv6/ethip6.c \
	core/ipv6/icmp6.c \
	core/ipv6/inet6.c \
	core/ipv6/ip6_addr.c \
	core/ipv6/ip6.c \
	core/ipv6/ip6_frag.c \
	core/ipv6/mld6.c \
	core/ipv6/nd6.c \
	netif/ethernet.c \
	netif/ppp/auth.c \
	netif/ppp/ccp.c \
	netif/ppp/chap-md5.c \
	netif/ppp/chap_ms.c \
	netif/ppp/chap-new.c \
	netif/ppp/demand.c \
	netif/ppp/eap.c \
	netif/ppp/ecp.c \
	netif/ppp/eui64.c \
	netif/ppp/fsm.c \
	netif/ppp/ipcp.c \
	netif/ppp/ipv6cp.c \
	netif/ppp/lcp.c \
	netif/ppp/magic.c \
	netif/ppp/mppe.c \
	netif/ppp/multilink.c \
	netif/ppp/polarssl/md5.c \
	netif/ppp/pppapi.c \
	netif/ppp/ppp.c \
	netif/ppp/pppcrypt.c \
	netif/ppp/pppoe.c \
	netif/ppp/pppol2tp.c \
	netif/ppp/pppos.c \
	netif/ppp/upap.c \
	netif/ppp/utils.c \
	netif/ppp/vj.c \
	)
ifeq ($(MICROPY_PY_LWIP_LOOPBACK),1)
CFLAGS_EXTMOD += -DLWIP_NETIF_LOOPBACK=1
endif
ifeq ($(MICROPY_PY_LWIP_SLIP),1)
CFLAGS_EXTMOD += -DMICROPY_PY_LWIP_SLIP=1
SRC_THIRDPARTY_C += $(LWIP_DIR)/netif/slipif.c
endif
endif

################################################################################
# btree

ifeq ($(MICROPY_PY_BTREE),1)
BTREE_DIR = lib/berkeley-db-1.xx
BERKELEY_DB_CONFIG_FILE ?= \"extmod/berkeley-db/berkeley_db_config_port.h\"
CFLAGS_EXTMOD += -DBERKELEY_DB_CONFIG_FILE=$(BERKELEY_DB_CONFIG_FILE)
CFLAGS_EXTMOD += $(BTREE_DEFS_EXTRA)
INC += -I$(TOP)/$(BTREE_DIR)/include
SRC_THIRDPARTY_C += $(addprefix $(BTREE_DIR)/,\
	btree/bt_close.c \
	btree/bt_conv.c \
	btree/bt_debug.c \
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
CFLAGS_EXTMOD += -DMICROPY_PY_BTREE=1
# we need to suppress certain warnings to get berkeley-db to compile cleanly
$(BUILD)/$(BTREE_DIR)/%.o: CFLAGS += -Wno-old-style-definition -Wno-sign-compare -Wno-unused-parameter -Wno-deprecated-non-prototype -Wno-unknown-warning-option
endif

################################################################################
# networking

ifeq ($(MICROPY_PY_NETWORK_CYW43),1)
CYW43_DIR = lib/cyw43-driver
GIT_SUBMODULES += $(CYW43_DIR)
CFLAGS_EXTMOD += -DMICROPY_PY_NETWORK_CYW43=1
SRC_THIRDPARTY_C += $(addprefix $(CYW43_DIR)/src/,\
	cyw43_bthci_uart.c \
	cyw43_ctrl.c \
	cyw43_lwip.c \
	cyw43_ll.c \
	cyw43_sdio.c \
	cyw43_spi.c \
	cyw43_stats.c \
	)

$(BUILD)/$(CYW43_DIR)/src/cyw43_%.o: CFLAGS += -std=c11
endif # MICROPY_PY_NETWORK_CYW43

ifneq ($(MICROPY_PY_NETWORK_WIZNET5K),)
ifneq ($(MICROPY_PY_NETWORK_WIZNET5K),0)
WIZNET5K_DIR=lib/wiznet5k
GIT_SUBMODULES += lib/wiznet5k
INC += -I$(TOP)/$(WIZNET5K_DIR) -I$(TOP)/$(WIZNET5K_DIR)/Ethernet
CFLAGS += -DMICROPY_PY_NETWORK_WIZNET5K=$(MICROPY_PY_NETWORK_WIZNET5K) -D_WIZCHIP_=$(MICROPY_PY_NETWORK_WIZNET5K)
CFLAGS_THIRDPARTY += -DWIZCHIP_PREFIXED_EXPORTS=1
ifeq ($(MICROPY_PY_LWIP),1)
# When using MACRAW mode (with lwIP), maximum buffer space must be used for the raw socket
CFLAGS_THIRDPARTY += -DWIZCHIP_USE_MAX_BUFFER
endif
SRC_THIRDPARTY_C += $(addprefix $(WIZNET5K_DIR)/,\
	Ethernet/W$(MICROPY_PY_NETWORK_WIZNET5K)/w$(MICROPY_PY_NETWORK_WIZNET5K).c \
	Ethernet/wizchip_conf.c \
	Ethernet/socket.c \
	Internet/DNS/dns.c \
	Internet/DHCP/dhcp.c \
	)
endif
endif # MICROPY_PY_NETWORK_WIZNET5K

ifeq ($(MICROPY_PY_NETWORK_ESP_HOSTED),1)
ESP_HOSTED_DIR = drivers/esp-hosted
PROTOBUF_C_DIR = lib/protobuf-c
PROTOC ?= protoc-c
GIT_SUBMODULES += $(PROTOBUF_C_DIR)

CFLAGS += -DMICROPY_PY_NETWORK_ESP_HOSTED=1
CFLAGS_EXTMOD += -DMICROPY_PY_NETWORK_ESP_HOSTED=1
INC += -I$(TOP)/$(ESP_HOSTED_DIR)

ESP_HOSTED_SRC_C = $(addprefix $(ESP_HOSTED_DIR)/,\
	esp_hosted_wifi.c \
	esp_hosted_netif.c \
	esp_hosted_hal.c \
	)

ifeq ($(MICROPY_PY_BLUETOOTH),1)
ESP_HOSTED_SRC_C += $(ESP_HOSTED_DIR)/esp_hosted_bthci_uart.c
endif

# Include the protobuf-c support functions
ESP_HOSTED_SRC_C += $(addprefix $(PROTOBUF_C_DIR)/,\
	protobuf-c/protobuf-c.c \
	)

$(BUILD)/$(PROTOBUF_C_DIR)/%.o: CFLAGS += -Wno-unused-but-set-variable

# Generate esp_hosted-pb-c.c|h from esp_hosted.proto
PROTO_GEN_SRC = $(BUILD)/extmod/esp_hosted.pb-c.c
ESP_HOSTED_SRC_C += $(PROTO_GEN_SRC)

$(PROTO_GEN_SRC): $(TOP)/$(ESP_HOSTED_DIR)/esp_hosted.proto
	$(PROTOC) --proto_path=$(dir $<) --c_out=$(dir $@) $<

# Scope the protobuf include paths to the esp_hosted source files, only
ESP_HOSTED_OBJS = $(addprefix $(BUILD)/, $(ESP_HOSTED_SRC_C:.c=.o))
$(ESP_HOSTED_OBJS): $(PROTO_GEN_SRC)
$(ESP_HOSTED_OBJS): CFLAGS += -I$(dir $(PROTO_GEN_SRC)) -I$(TOP)/$(PROTOBUF_C_DIR)

DRIVERS_SRC_C += $(ESP_HOSTED_SRC_C)

endif # MICROPY_PY_NETWORK_ESP_HOSTED

################################################################################
# bluetooth

ifeq ($(MICROPY_PY_BLUETOOTH),1)
CFLAGS_EXTMOD += -DMICROPY_PY_BLUETOOTH=1

ifeq ($(MICROPY_BLUETOOTH_NIMBLE),1)
ifeq ($(MICROPY_BLUETOOTH_BTSTACK),1)
$(error Cannot enable both NimBLE and BTstack at the same time)
endif
endif

ifneq ($(MICROPY_BLUETOOTH_NIMBLE),1)
ifneq ($(MICROPY_BLUETOOTH_BTSTACK),1)
$(error Must enable one of MICROPY_BLUETOOTH_NIMBLE or MICROPY_BLUETOOTH_BTSTACK)
endif
endif

ifeq ($(MICROPY_BLUETOOTH_NIMBLE),1)
include $(TOP)/extmod/nimble/nimble.mk
endif

ifeq ($(MICROPY_BLUETOOTH_BTSTACK),1)
include $(TOP)/extmod/btstack/btstack.mk
endif

endif

################################################################################
# openamp

ifeq ($(MICROPY_PY_OPENAMP),1)
OPENAMP_DIR = lib/open-amp
LIBMETAL_DIR = lib/libmetal
GIT_SUBMODULES += $(LIBMETAL_DIR) $(OPENAMP_DIR)
MICROPY_PY_OPENAMP_MODE ?= 0
include $(TOP)/extmod/libmetal/libmetal.mk

INC += -I$(TOP)/$(OPENAMP_DIR)
CFLAGS += -DMICROPY_PY_OPENAMP=1

ifeq ($(MICROPY_PY_OPENAMP_REMOTEPROC),1)
CFLAGS += -DMICROPY_PY_OPENAMP_REMOTEPROC=1
endif

ifeq ($(MICROPY_PY_OPENAMP_MODE),0)
CFLAGS += -DMICROPY_PY_OPENAMP_HOST=1
CFLAGS_THIRDPARTY += -DVIRTIO_DRIVER_ONLY
else ifeq ($(MICROPY_PY_OPENAMP_MODE),1)
CFLAGS += -DMICROPY_PY_OPENAMP_DEVICE=1
CFLAGS_THIRDPARTY += -DVIRTIO_DEVICE_ONLY
else
$(error Invalid Open-AMP mode specified: $(MICROPY_PY_OPENAMP_MODE))
endif

CFLAGS_THIRDPARTY += \
    -I$(BUILD)/openamp \
    -I$(TOP)/$(OPENAMP_DIR) \
    -I$(TOP)/$(OPENAMP_DIR)/lib/include/ \
    -DMETAL_INTERNAL \
    -DNO_ATOMIC_64_SUPPORT \
    -DRPMSG_BUFFER_SIZE=512 \

# OpenAMP's source files.
SRC_OPENAMP_C += $(addprefix $(OPENAMP_DIR)/lib/,\
	rpmsg/rpmsg.c \
	rpmsg/rpmsg_virtio.c \
	virtio/virtio.c \
	virtio/virtqueue.c \
	virtio_mmio/virtio_mmio_drv.c \
	)

# OpenAMP's remoteproc source files.
ifeq ($(MICROPY_PY_OPENAMP_REMOTEPROC),1)
SRC_OPENAMP_C += $(addprefix $(OPENAMP_DIR)/lib/remoteproc/,\
	elf_loader.c \
	remoteproc.c \
	remoteproc_virtio.c \
	rsc_table_parser.c \
	)
endif # MICROPY_PY_OPENAMP_REMOTEPROC

# Disable compiler warnings in OpenAMP (variables used only for assert).
$(BUILD)/$(OPENAMP_DIR)/lib/rpmsg/rpmsg_virtio.o: CFLAGS += -Wno-unused-but-set-variable
$(BUILD)/$(OPENAMP_DIR)/lib/virtio_mmio/virtio_mmio_drv.o: CFLAGS += -Wno-unused-but-set-variable

# We need to have generated libmetal before compiling OpenAMP.
$(addprefix $(BUILD)/, $(SRC_OPENAMP_C:.c=.o)): $(BUILD)/openamp/metal/config.h

SRC_THIRDPARTY_C += $(SRC_OPENAMP_C) $(SRC_LIBMETAL_C:$(BUILD)/%=%)

endif # MICROPY_PY_OPENAMP

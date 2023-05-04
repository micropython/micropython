# This makefile fragment adds the source code files for the core extmod modules
# and provides rules to build 3rd-party components for extmod modules.

SRC_EXTMOD_C += \
	extmod/machine_bitstream.c \
	extmod/machine_i2c.c \
	extmod/machine_mem.c \
	extmod/machine_pinbase.c \
	extmod/machine_pulse.c \
	extmod/machine_pwm.c \
	extmod/machine_signal.c \
	extmod/machine_spi.c \
	extmod/machine_timer.c \
	extmod/modbluetooth.c \
	extmod/modbtree.c \
	extmod/modframebuf.c \
	extmod/modlwip.c \
	extmod/modnetwork.c \
	extmod/modonewire.c \
	extmod/moduasyncio.c \
	extmod/modubinascii.c \
	extmod/moducryptolib.c \
	extmod/moductypes.c \
	extmod/moduhashlib.c \
	extmod/moduheapq.c \
	extmod/modujson.c \
	extmod/moduos.c \
	extmod/moduplatform.c\
	extmod/modurandom.c \
	extmod/modure.c \
	extmod/moduselect.c \
	extmod/modusocket.c \
	extmod/modussl_axtls.c \
	extmod/modussl_mbedtls.c \
	extmod/modutime.c \
	extmod/modutimeq.c \
	extmod/moduwebsocket.c \
	extmod/moduzlib.c \
	extmod/modwebrepl.c \
	extmod/network_cyw43.c \
	extmod/network_lwip.c \
	extmod/network_ninaw10.c \
	extmod/network_wiznet5k.c \
	extmod/uos_dupterm.c \
	extmod/vfs.c \
	extmod/vfs_blockdev.c \
	extmod/vfs_fat.c \
	extmod/vfs_fat_diskio.c \
	extmod/vfs_fat_file.c \
	extmod/vfs_lfs.c \
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
CFLAGS_THIRDPARTY += -DLFS2_NO_MALLOC -DLFS2_NO_DEBUG -DLFS2_NO_WARN -DLFS2_NO_ERROR -DLFS2_NO_ASSERT
SRC_THIRDPARTY_C += $(addprefix $(LITTLEFS_DIR)/,\
	lfs2.c \
	lfs2_util.c \
	)

$(BUILD)/$(LITTLEFS_DIR)/lfs2.o: CFLAGS += -Wno-missing-field-initializers
endif

################################################################################
# ussl

ifeq ($(MICROPY_PY_USSL),1)
CFLAGS_EXTMOD += -DMICROPY_PY_USSL=1
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
MBEDTLS_CONFIG_FILE ?= \"mbedtls/mbedtls_config.h\"
GIT_SUBMODULES += $(MBEDTLS_DIR)
CFLAGS_EXTMOD += -DMBEDTLS_CONFIG_FILE=$(MBEDTLS_CONFIG_FILE)
CFLAGS_EXTMOD += -DMICROPY_SSL_MBEDTLS=1 -I$(TOP)/$(MBEDTLS_DIR)/include
SRC_THIRDPARTY_C += lib/mbedtls_errors/mp_mbedtls_errors.c
SRC_THIRDPARTY_C += $(addprefix $(MBEDTLS_DIR)/library/,\
	aes.c \
	aesni.c \
	arc4.c \
	asn1parse.c \
	asn1write.c \
	base64.c \
	bignum.c \
	blowfish.c \
	camellia.c \
	ccm.c \
	certs.c \
	chacha20.c \
	chachapoly.c \
	cipher.c \
	cipher_wrap.c \
	cmac.c \
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
	havege.c \
	hmac_drbg.c \
	md2.c \
	md4.c \
	md5.c \
	md.c \
	oid.c \
	padlock.c \
	pem.c \
	pk.c \
	pkcs11.c \
	pkcs12.c \
	pkcs5.c \
	pkparse.c \
	pk_wrap.c \
	pkwrite.c \
	platform.c \
	platform_util.c \
	poly1305.c \
	ripemd160.c \
	rsa.c \
	rsa_internal.c \
	sha1.c \
	sha256.c \
	sha512.c \
	ssl_cache.c \
	ssl_ciphersuites.c \
	ssl_cli.c \
	ssl_cookie.c \
	ssl_srv.c \
	ssl_msg.c \
	ssl_ticket.c \
	ssl_tls.c \
	timing.c \
	constant_time.c \
	x509.c \
	x509_create.c \
	x509_crl.c \
	x509_crt.c \
	x509_csr.c \
	x509write_crt.c \
	x509write_csr.c \
	xtea.c \
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
	)
ifeq ($(MICROPY_PY_LWIP_SLIP),1)
CFLAGS_EXTMOD += -DMICROPY_PY_LWIP_SLIP=1
SRC_THIRDPARTY_C += $(LWIP_DIR)/netif/slipif.c
endif
endif

################################################################################
# btree

ifeq ($(MICROPY_PY_BTREE),1)
BTREE_DIR = lib/berkeley-db-1.xx
BTREE_DEFS = -D__DBINTERFACE_PRIVATE=1 -Dmpool_error=printf -Dabort=abort_ "-Dvirt_fd_t=void*" $(BTREE_DEFS_EXTRA)
INC += -I$(TOP)/$(BTREE_DIR)/PORT/include
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
# and we have separate BTREE_DEFS so the definitions don't interfere with other source code
$(BUILD)/$(BTREE_DIR)/%.o: CFLAGS += -Wno-old-style-definition -Wno-sign-compare -Wno-unused-parameter $(BTREE_DEFS)
$(BUILD)/extmod/modbtree.o: CFLAGS += $(BTREE_DEFS)
endif

################################################################################
# networking

ifeq ($(MICROPY_PY_NETWORK_CYW43),1)
CYW43_DIR = lib/cyw43-driver
GIT_SUBMODULES += $(CYW43_DIR)
CFLAGS_EXTMOD += -DMICROPY_PY_NETWORK_CYW43=1
SRC_THIRDPARTY_C += $(addprefix $(CYW43_DIR)/src/,\
	cyw43_ctrl.c \
	cyw43_lwip.c \
	cyw43_ll.c \
	cyw43_sdio.c \
	cyw43_stats.c \
	)
ifeq ($(MICROPY_PY_BLUETOOTH),1)
DRIVERS_SRC_C += drivers/cyw43/cywbt.c
endif

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
endif

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

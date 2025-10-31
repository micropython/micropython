# Makefile directives for Apache Mynewt NimBLE component

ifeq ($(MICROPY_BLUETOOTH_NIMBLE),1)

EXTMOD_DIR = extmod
NIMBLE_EXTMOD_DIR = $(EXTMOD_DIR)/nimble

SRC_EXTMOD_C += $(NIMBLE_EXTMOD_DIR)/modbluetooth_nimble.c

CFLAGS_EXTMOD += -DMICROPY_BLUETOOTH_NIMBLE=1

# Use NimBLE from the submodule in lib/mynewt-nimble by default,
# allowing a port to use their own system version (e.g. ESP32).
MICROPY_BLUETOOTH_NIMBLE_BINDINGS_ONLY ?= 0

CFLAGS_EXTMOD += -DMICROPY_BLUETOOTH_NIMBLE_BINDINGS_ONLY=$(MICROPY_BLUETOOTH_NIMBLE_BINDINGS_ONLY)

ifeq ($(MICROPY_BLUETOOTH_NIMBLE_BINDINGS_ONLY),0)

GIT_SUBMODULES += lib/mynewt-nimble

# On all ports where we provide the full implementation (i.e. not just
# bindings like on ESP32), then we don't need to use the ringbuffer. In this
# case, all NimBLE events are run by the MicroPython scheduler. On Unix, the
# scheduler is also responsible for polling the UART, whereas on STM32 the
# UART is also polled by the RX IRQ.
CFLAGS_EXTMOD += -DMICROPY_PY_BLUETOOTH_USE_SYNC_EVENTS=1

# Without the ringbuffer, and with the full implementation, we can also
# enable pairing and bonding. This requires both synchronous events and
# some customisation of the key store.
CFLAGS_EXTMOD += -DMICROPY_PY_BLUETOOTH_ENABLE_PAIRING_BONDING=1

NIMBLE_LIB_DIR = lib/mynewt-nimble

SRC_THIRDPARTY_C += $(addprefix $(NIMBLE_LIB_DIR)/, \
	$(addprefix ext/tinycrypt/src/, \
		aes_encrypt.c \
		cmac_mode.c \
		ecc.c \
		ecc_dh.c \
		utils.c \
		) \
	nimble/host/services/gap/src/ble_svc_gap.c \
	nimble/host/services/gatt/src/ble_svc_gatt.c \
	$(addprefix nimble/host/src/, \
		ble_att.c \
		ble_att_clt.c \
		ble_att_cmd.c \
		ble_att_svr.c \
		ble_eddystone.c \
		ble_gap.c \
		ble_gattc.c \
		ble_gatts.c \
		ble_hs_adv.c \
		ble_hs_atomic.c \
		ble_hs.c \
		ble_hs_cfg.c \
		ble_hs_conn.c \
		ble_hs_flow.c \
		ble_hs_hci.c \
		ble_hs_hci_cmd.c \
		ble_hs_hci_evt.c \
		ble_hs_hci_util.c \
		ble_hs_id.c \
		ble_hs_log.c \
		ble_hs_mbuf.c \
		ble_hs_misc.c \
		ble_hs_mqueue.c \
		ble_hs_pvcy.c \
		ble_hs_startup.c \
		ble_hs_stop.c \
		ble_ibeacon.c \
		ble_l2cap.c \
		ble_l2cap_coc.c \
		ble_l2cap_sig.c \
		ble_l2cap_sig_cmd.c \
		ble_monitor.c \
		ble_sm_alg.c \
		ble_sm.c \
		ble_sm_cmd.c \
		ble_sm_lgcy.c \
		ble_sm_sc.c \
		ble_store.c \
		ble_store_util.c \
		ble_uuid.c \
		) \
	nimble/host/util/src/addr.c \
	$(addprefix porting/nimble/src/, \
		endian.c \
		mem.c \
		nimble_port.c \
		os_mbuf.c \
		os_mempool.c \
		os_msys_init.c \
		) \
	)
	# nimble/host/store/ram/src/ble_store_ram.c \

SRC_THIRDPARTY_C += $(addprefix $(NIMBLE_EXTMOD_DIR)/, \
	nimble/nimble_npl_os.c \
	)

INC += -I$(TOP)/$(NIMBLE_EXTMOD_DIR)
INC += -I$(TOP)/$(NIMBLE_LIB_DIR)
INC += -I$(TOP)/$(NIMBLE_LIB_DIR)/ext/tinycrypt/include
INC += -I$(TOP)/$(NIMBLE_LIB_DIR)/nimble/host/include
INC += -I$(TOP)/$(NIMBLE_LIB_DIR)/nimble/host/services/gap/include
INC += -I$(TOP)/$(NIMBLE_LIB_DIR)/nimble/host/services/gatt/include
INC += -I$(TOP)/$(NIMBLE_LIB_DIR)/nimble/host/store/ram/include
INC += -I$(TOP)/$(NIMBLE_LIB_DIR)/nimble/host/util/include
INC += -I$(TOP)/$(NIMBLE_LIB_DIR)/nimble/include
INC += -I$(TOP)/$(NIMBLE_LIB_DIR)/porting/nimble/include

$(BUILD)/$(NIMBLE_LIB_DIR)/%.o: CFLAGS += -Wno-maybe-uninitialized -Wno-pointer-arith -Wno-unused-but-set-variable -Wno-format -Wno-sign-compare -Wno-old-style-declaration

endif

ifeq ($(MICROPY_BLUETOOTH_NIMBLE_CONTROLLER),1)

# Include controller layer to run entire stack on-chip
CFLAGS_EXTMOD += -DMICROPY_BLUETOOTH_NIMBLE_CONTROLLER=1

INC += -I$(TOP)/$(NIMBLE_LIB_DIR)/nimble/controller/include

SRC_THIRDPARTY_C += $(addprefix $(NIMBLE_LIB_DIR)/, \
	$(addprefix nimble/controller/src/, \
		ble_ll.c \
		ble_ll_adv.c \
		ble_ll_conn.c \
		ble_ll_conn_hci.c \
		ble_ll_ctrl.c \
		ble_ll_dtm.c \
		ble_ll_hci.c \
		ble_ll_hci_ev.c \
		ble_ll_iso.c \
		ble_ll_rand.c \
		ble_ll_resolv.c \
		ble_ll_rfmgmt.c \
		ble_ll_scan.c \
		ble_ll_sched.c \
		ble_ll_supp_cmd.c \
		ble_ll_sync.c \
		ble_ll_trace.c \
		ble_ll_utils.c \
		ble_ll_whitelist.c \
	) \
)

SRC_THIRDPARTY_C += $(addprefix $(NIMBLE_LIB_DIR)/, \
	$(addprefix porting/nimble/src/, \
		hal_timer.c \
		os_cputime.c \
		os_cputime_pwr2.c \
	) \
)

SRC_THIRDPARTY_C += $(NIMBLE_LIB_DIR)/nimble/transport/ram/src/ble_hci_ram.c
INC += -I$(TOP)/$(NIMBLE_LIB_DIR)/nimble/transport/ram/include

else # !MICROPY_BLUETOOTH_NIMBLE_CONTROLLER
# External controller being used
INC += -I$(TOP)/$(NIMBLE_LIB_DIR)/nimble/transport/uart/include
SRC_THIRDPARTY_C += $(addprefix $(NIMBLE_EXTMOD_DIR)/, \
	hal/hal_uart.c \
	)
SRC_THIRDPARTY_C += $(NIMBLE_LIB_DIR)/nimble/transport/uart/src/ble_hci_uart.c
endif
endif

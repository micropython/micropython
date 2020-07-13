# Makefile directives for Apache Mynewt NimBLE component

ifeq ($(MICROPY_BLUETOOTH_NIMBLE),1)

EXTMOD_SRC_C += extmod/nimble/modbluetooth_nimble.c

CFLAGS_MOD += -DMICROPY_BLUETOOTH_NIMBLE=1

NIMBLE_EXTMOD_DIR = extmod/nimble

# Use NimBLE from the submodule in lib/mynewt-nimble by default,
# allowing a port to use their own system version (e.g. ESP32).
MICROPY_BLUETOOTH_NIMBLE_BINDINGS_ONLY ?= 0

ifeq ($(MICROPY_BLUETOOTH_NIMBLE_BINDINGS_ONLY),0)

NIMBLE_LIB_DIR = lib/mynewt-nimble

LIB_SRC_C += $(addprefix $(NIMBLE_LIB_DIR)/, \
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
	nimble/host/store/ram/src/ble_store_ram.c \
	nimble/host/util/src/addr.c \
	nimble/transport/uart/src/ble_hci_uart.c \
	$(addprefix porting/nimble/src/, \
		endian.c \
		mem.c \
		nimble_port.c \
		os_mbuf.c \
		os_mempool.c \
		os_msys_init.c \
		) \
	)

EXTMOD_SRC_C += $(addprefix $(NIMBLE_EXTMOD_DIR)/, \
	nimble/npl_os.c \
	hal/hal_uart.c \
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
INC += -I$(TOP)/$(NIMBLE_LIB_DIR)/nimble/transport/uart/include
INC += -I$(TOP)/$(NIMBLE_LIB_DIR)/porting/nimble/include

$(BUILD)/$(NIMBLE_LIB_DIR)/%.o: CFLAGS += -Wno-maybe-uninitialized -Wno-pointer-arith -Wno-unused-but-set-variable -Wno-format

endif

endif

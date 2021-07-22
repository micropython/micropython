set(NIMBLE_LIB_DIR "${MICROPY_DIR}/lib/mynewt-nimble")
set(NIMBLE_EXTMOD_DIR "${MICROPY_DIR}/extmod/nimble")

add_library(micropy_extmod_nimble INTERFACE)

target_include_directories(micropy_extmod_nimble INTERFACE
    ${MICROPY_DIR}/
    ${MICROPY_PORT_DIR}/
    ${NIMBLE_EXTMOD_DIR}/
    ${NIMBLE_LIB_DIR}/
    ${NIMBLE_LIB_DIR}/ext/tinycrypt/include
    ${NIMBLE_LIB_DIR}/nimble/host/include
    ${NIMBLE_LIB_DIR}/nimble/host/services/gap/include
    ${NIMBLE_LIB_DIR}/nimble/host/services/gatt/include
    ${NIMBLE_LIB_DIR}/nimble/host/store/ram/include
    ${NIMBLE_LIB_DIR}/nimble/host/util/include
    ${NIMBLE_LIB_DIR}/nimble/include
    ${NIMBLE_LIB_DIR}/nimble/transport/uart/include
    ${NIMBLE_LIB_DIR}/porting/nimble/include
)

target_sources(micropy_extmod_nimble INTERFACE
    ${NIMBLE_LIB_DIR}/ext/tinycrypt/src/aes_encrypt.c
    ${NIMBLE_LIB_DIR}/ext/tinycrypt/src/cmac_mode.c
    ${NIMBLE_LIB_DIR}/ext/tinycrypt/src/ecc.c
    ${NIMBLE_LIB_DIR}/ext/tinycrypt/src/ecc_dh.c
    ${NIMBLE_LIB_DIR}/ext/tinycrypt/src/utils.c
    ${NIMBLE_LIB_DIR}/nimble/host/services/gap/src/ble_svc_gap.c
    ${NIMBLE_LIB_DIR}/nimble/host/services/gatt/src/ble_svc_gatt.c
    ${NIMBLE_LIB_DIR}/nimble/host/src/ble_att.c
    ${NIMBLE_LIB_DIR}/nimble/host/src/ble_att_clt.c
    ${NIMBLE_LIB_DIR}/nimble/host/src/ble_att_cmd.c
    ${NIMBLE_LIB_DIR}/nimble/host/src/ble_att_svr.c
    ${NIMBLE_LIB_DIR}/nimble/host/src/ble_eddystone.c
    ${NIMBLE_LIB_DIR}/nimble/host/src/ble_gap.c
    ${NIMBLE_LIB_DIR}/nimble/host/src/ble_gattc.c
    ${NIMBLE_LIB_DIR}/nimble/host/src/ble_gatts.c
    ${NIMBLE_LIB_DIR}/nimble/host/src/ble_hs_adv.c
    ${NIMBLE_LIB_DIR}/nimble/host/src/ble_hs_atomic.c
    ${NIMBLE_LIB_DIR}/nimble/host/src/ble_hs.c
    ${NIMBLE_LIB_DIR}/nimble/host/src/ble_hs_cfg.c
    ${NIMBLE_LIB_DIR}/nimble/host/src/ble_hs_conn.c
    ${NIMBLE_LIB_DIR}/nimble/host/src/ble_hs_flow.c
    ${NIMBLE_LIB_DIR}/nimble/host/src/ble_hs_hci.c
    ${NIMBLE_LIB_DIR}/nimble/host/src/ble_hs_hci_cmd.c
    ${NIMBLE_LIB_DIR}/nimble/host/src/ble_hs_hci_evt.c
    ${NIMBLE_LIB_DIR}/nimble/host/src/ble_hs_hci_util.c
    ${NIMBLE_LIB_DIR}/nimble/host/src/ble_hs_id.c
    ${NIMBLE_LIB_DIR}/nimble/host/src/ble_hs_log.c
    ${NIMBLE_LIB_DIR}/nimble/host/src/ble_hs_mbuf.c
    ${NIMBLE_LIB_DIR}/nimble/host/src/ble_hs_misc.c
    ${NIMBLE_LIB_DIR}/nimble/host/src/ble_hs_mqueue.c
    ${NIMBLE_LIB_DIR}/nimble/host/src/ble_hs_pvcy.c
    ${NIMBLE_LIB_DIR}/nimble/host/src/ble_hs_startup.c
    ${NIMBLE_LIB_DIR}/nimble/host/src/ble_hs_stop.c
    ${NIMBLE_LIB_DIR}/nimble/host/src/ble_ibeacon.c
    ${NIMBLE_LIB_DIR}/nimble/host/src/ble_l2cap.c
    ${NIMBLE_LIB_DIR}/nimble/host/src/ble_l2cap_coc.c
    ${NIMBLE_LIB_DIR}/nimble/host/src/ble_l2cap_sig.c
    ${NIMBLE_LIB_DIR}/nimble/host/src/ble_l2cap_sig_cmd.c
    ${NIMBLE_LIB_DIR}/nimble/host/src/ble_monitor.c
    ${NIMBLE_LIB_DIR}/nimble/host/src/ble_sm_alg.c
    ${NIMBLE_LIB_DIR}/nimble/host/src/ble_sm.c
    ${NIMBLE_LIB_DIR}/nimble/host/src/ble_sm_cmd.c
    ${NIMBLE_LIB_DIR}/nimble/host/src/ble_sm_lgcy.c
    ${NIMBLE_LIB_DIR}/nimble/host/src/ble_sm_sc.c
    ${NIMBLE_LIB_DIR}/nimble/host/src/ble_store.c
    ${NIMBLE_LIB_DIR}/nimble/host/src/ble_store_util.c
    ${NIMBLE_LIB_DIR}/nimble/host/src/ble_uuid.c
    ${NIMBLE_LIB_DIR}/nimble/host/util/src/addr.c
    ${NIMBLE_LIB_DIR}/nimble/transport/uart/src/ble_hci_uart.c
    ${NIMBLE_LIB_DIR}/porting/nimble/src/endian.c
    ${NIMBLE_LIB_DIR}/porting/nimble/src/mem.c
    ${NIMBLE_LIB_DIR}/porting/nimble/src/nimble_port.c
    ${NIMBLE_LIB_DIR}/porting/nimble/src/os_mbuf.c
    ${NIMBLE_LIB_DIR}/porting/nimble/src/os_mempool.c
    ${NIMBLE_LIB_DIR}/porting/nimble/src/os_msys_init.c
    ${NIMBLE_EXTMOD_DIR}/nimble/nimble_npl_os.c
    ${NIMBLE_EXTMOD_DIR}/hal/hal_uart.c

)

set(BTSTACK_LIB_DIR "${MICROPY_DIR}/lib/btstack")
set(BTSTACK_EXTMOD_DIR "${MICROPY_DIR}/extmod/btstack")

add_library(micropy_extmod_btstack INTERFACE)

target_include_directories(micropy_extmod_btstack INTERFACE
    ${MICROPY_DIR}/
    ${MICROPY_PORT_DIR}/
    ${BTSTACK_EXTMOD_DIR}/
    ${BTSTACK_LIB_DIR}/src
    ${BTSTACK_LIB_DIR}/3rd-party/bluedroid/decoder/include
    ${BTSTACK_LIB_DIR}/3rd-party/bluedroid/encoder/include
    ${BTSTACK_LIB_DIR}/3rd-party/md5
    ${BTSTACK_LIB_DIR}/3rd-party/yxml
)

target_sources(micropy_extmod_btstack INTERFACE
    ${BTSTACK_LIB_DIR}/platform/embedded/hci_dump_embedded_stdout.c
    ${BTSTACK_LIB_DIR}/src/ad_parser.c
    ${BTSTACK_LIB_DIR}/src/ble/gatt-service/ancs_client.c
    ${BTSTACK_LIB_DIR}/src/ble/att_db.c
    ${BTSTACK_LIB_DIR}/src/ble/att_db_util.c
    ${BTSTACK_LIB_DIR}/src/ble/att_dispatch.c
    ${BTSTACK_LIB_DIR}/src/ble/att_server.c
    ${BTSTACK_LIB_DIR}/src/ble/gatt-service/battery_service_server.c
    ${BTSTACK_LIB_DIR}/src/ble/gatt-service/cycling_power_service_server.c
    ${BTSTACK_LIB_DIR}/src/ble/gatt-service/cycling_speed_and_cadence_service_server.c
    ${BTSTACK_LIB_DIR}/src/ble/gatt-service/device_information_service_server.c
    ${BTSTACK_LIB_DIR}/src/ble/gatt-service/heart_rate_service_server.c
    ${BTSTACK_LIB_DIR}/src/ble/gatt-service/hids_device.c
    ${BTSTACK_LIB_DIR}/src/mesh/gatt-service/mesh_provisioning_service_server.c
    ${BTSTACK_LIB_DIR}/src/mesh/gatt-service/mesh_proxy_service_server.c
    ${BTSTACK_LIB_DIR}/src/ble/gatt-service/nordic_spp_service_server.c
    ${BTSTACK_LIB_DIR}/src/ble/gatt-service/ublox_spp_service_server.c
    ${BTSTACK_LIB_DIR}/src/ble/gatt_client.c
    ${BTSTACK_LIB_DIR}/src/ble/le_device_db_memory.c
    ${BTSTACK_LIB_DIR}/src/ble/sm.c
    ${BTSTACK_LIB_DIR}/src/btstack_audio.c
    ${BTSTACK_LIB_DIR}/src/btstack_base64_decoder.c
    ${BTSTACK_LIB_DIR}/src/btstack_crypto.c
    ${BTSTACK_LIB_DIR}/src/btstack_hid_parser.c
    ${BTSTACK_LIB_DIR}/src/btstack_linked_list.c
    ${BTSTACK_LIB_DIR}/src/btstack_memory.c
    ${BTSTACK_LIB_DIR}/src/btstack_memory_pool.c
    ${BTSTACK_LIB_DIR}/src/btstack_resample.c
    ${BTSTACK_LIB_DIR}/src/btstack_ring_buffer.c
    ${BTSTACK_LIB_DIR}/src/btstack_run_loop.c
    ${BTSTACK_LIB_DIR}/src/btstack_run_loop_base.c
    ${BTSTACK_LIB_DIR}/src/btstack_slip.c
    ${BTSTACK_LIB_DIR}/src/btstack_tlv.c
    ${BTSTACK_LIB_DIR}/src/btstack_tlv_none.c
    ${BTSTACK_LIB_DIR}/src/btstack_util.c
    ${BTSTACK_LIB_DIR}/src/hci.c
    ${BTSTACK_LIB_DIR}/src/hci_cmd.c
    ${BTSTACK_LIB_DIR}/src/hci_dump.c
    ${BTSTACK_LIB_DIR}/src/hci_transport_em9304_spi.c
    ${BTSTACK_LIB_DIR}/src/hci_transport_h4.c
    ${BTSTACK_LIB_DIR}/src/l2cap.c
    ${BTSTACK_LIB_DIR}/src/l2cap_signaling.c
)

# CMake fragment for MicroPython extmod component

set(MICROPY_EXTMOD_DIR "${MICROPY_DIR}/extmod")
set(MICROPY_OOFATFS_DIR "${MICROPY_DIR}/lib/oofatfs")

set(MICROPY_SOURCE_EXTMOD
    ${MICROPY_EXTMOD_DIR}/machine_i2c.c
    ${MICROPY_EXTMOD_DIR}/machine_mem.c
    ${MICROPY_EXTMOD_DIR}/machine_pulse.c
    ${MICROPY_EXTMOD_DIR}/machine_signal.c
    ${MICROPY_EXTMOD_DIR}/machine_spi.c
    ${MICROPY_EXTMOD_DIR}/modbtree.c
    ${MICROPY_EXTMOD_DIR}/modframebuf.c
    ${MICROPY_EXTMOD_DIR}/moduasyncio.c
    ${MICROPY_EXTMOD_DIR}/modubinascii.c
    ${MICROPY_EXTMOD_DIR}/moducryptolib.c
    ${MICROPY_EXTMOD_DIR}/moductypes.c
    ${MICROPY_EXTMOD_DIR}/moduhashlib.c
    ${MICROPY_EXTMOD_DIR}/moduheapq.c
    ${MICROPY_EXTMOD_DIR}/modujson.c
    ${MICROPY_EXTMOD_DIR}/modurandom.c
    ${MICROPY_EXTMOD_DIR}/modure.c
    ${MICROPY_EXTMOD_DIR}/moduselect.c
    ${MICROPY_EXTMOD_DIR}/modussl_axtls.c
    ${MICROPY_EXTMOD_DIR}/modussl_mbedtls.c
    ${MICROPY_EXTMOD_DIR}/modutimeq.c
    ${MICROPY_EXTMOD_DIR}/moduwebsocket.c
    ${MICROPY_EXTMOD_DIR}/moduzlib.c
    ${MICROPY_EXTMOD_DIR}/modwebrepl.c
    ${MICROPY_EXTMOD_DIR}/uos_dupterm.c
    ${MICROPY_EXTMOD_DIR}/utime_mphal.c
    ${MICROPY_EXTMOD_DIR}/vfs.c
    ${MICROPY_EXTMOD_DIR}/vfs_blockdev.c
    ${MICROPY_EXTMOD_DIR}/vfs_fat.c
    ${MICROPY_EXTMOD_DIR}/vfs_fat_diskio.c
    ${MICROPY_EXTMOD_DIR}/vfs_fat_file.c
    ${MICROPY_EXTMOD_DIR}/vfs_lfs.c
    ${MICROPY_EXTMOD_DIR}/vfs_posix.c
    ${MICROPY_EXTMOD_DIR}/vfs_posix_file.c
    ${MICROPY_EXTMOD_DIR}/vfs_reader.c
    ${MICROPY_EXTMOD_DIR}/virtpin.c
)

set (MICROPY_INCLUDE_EXTMOD)

if(NOT TOOTHLESS)
    set(MICROPY_SOURCE_EXTMOD ${MICROPY_SOURCE_EXTMOD}
        ${MICROPY_EXTMOD_DIR}/modbluetooth.c
        ${MICROPY_EXTMOD_DIR}/nimble/modbluetooth_nimble.c
        ${MICROPY_EXTMOD_DIR}/nimble/hal/hal_uart.c
    )

    set (MICROPY_INCLUDE_EXTMOD ${MICROPY_INCLUDE_EXTMOD}
        ${IDF_PATH}/components/bt/host/nimble/nimble/nimble/host/include
        ${IDF_PATH}/components/bt/host/nimble/esp-hci/include/
        ${IDF_PATH}/components/bt/host/nimble/nimble/nimble/include
        ${IDF_PATH}/components/bt/host/nimble/nimble/porting/nimble/include
        ${IDF_PATH}/components/bt/host/nimble/port/include
        ${IDF_PATH}/components/bt/host/nimble/nimble/porting/npl/freertos/include
        ${IDF_PATH}/components/bt/common/osi/include/osi
        ${IDF_PATH}/components/bt/host/nimble/nimble/nimble/host/util/include
        ${IDF_PATH}/components/bt/host/nimble/nimble/porting/npl/freertos/include
        ${IDF_PATH}/components/bt/host/nimble/nimble/nimble/host/services/gap/include
        ${IDF_PATH}/components/bt/host/nimble/nimble/nimble/host/services/gatt/include
        ${IDF_PATH}/components/bt/host/nimble/nimble/nimble/transport/uart/include
    )
endif()

#string(JOIN " " __MSE ${MICROPY_SOURCE_EXTMOD})
#message(NOTICE ******************** MICROPY_SOURCE_EXTMOD ******* ${__MSE})

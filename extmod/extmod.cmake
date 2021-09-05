# CMake fragment for MicroPython extmod component

set(MICROPY_EXTMOD_DIR "${MICROPY_DIR}/extmod")
set(MICROPY_OOFATFS_DIR "${MICROPY_DIR}/lib/oofatfs")

set(MICROPY_SOURCE_EXTMOD
    ${MICROPY_DIR}/shared/libc/abort_.c
    ${MICROPY_DIR}/shared/libc/printf.c
    ${MICROPY_EXTMOD_DIR}/machine_bitstream.c
    ${MICROPY_EXTMOD_DIR}/machine_i2c.c
    ${MICROPY_EXTMOD_DIR}/machine_mem.c
    ${MICROPY_EXTMOD_DIR}/machine_pulse.c
    ${MICROPY_EXTMOD_DIR}/machine_signal.c
    ${MICROPY_EXTMOD_DIR}/machine_spi.c
    ${MICROPY_EXTMOD_DIR}/modbluetooth.c
    ${MICROPY_EXTMOD_DIR}/modbtree.c
    ${MICROPY_EXTMOD_DIR}/modframebuf.c
    ${MICROPY_EXTMOD_DIR}/modonewire.c
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
    ${MICROPY_EXTMOD_DIR}/nimble/modbluetooth_nimble.c
)

# Library for btree module and associated code

set(MICROPY_LIB_BERKELEY_DIR "${MICROPY_DIR}/lib/berkeley-db-1.xx")

if(EXISTS "${MICROPY_LIB_BERKELEY_DIR}/btree/bt_close.c")
    add_library(micropy_extmod_btree OBJECT
        ${MICROPY_LIB_BERKELEY_DIR}/btree/bt_close.c
        ${MICROPY_LIB_BERKELEY_DIR}/btree/bt_conv.c
        ${MICROPY_LIB_BERKELEY_DIR}/btree/bt_debug.c
        ${MICROPY_LIB_BERKELEY_DIR}/btree/bt_delete.c
        ${MICROPY_LIB_BERKELEY_DIR}/btree/bt_get.c
        ${MICROPY_LIB_BERKELEY_DIR}/btree/bt_open.c
        ${MICROPY_LIB_BERKELEY_DIR}/btree/bt_overflow.c
        ${MICROPY_LIB_BERKELEY_DIR}/btree/bt_page.c
        ${MICROPY_LIB_BERKELEY_DIR}/btree/bt_put.c
        ${MICROPY_LIB_BERKELEY_DIR}/btree/bt_search.c
        ${MICROPY_LIB_BERKELEY_DIR}/btree/bt_seq.c
        ${MICROPY_LIB_BERKELEY_DIR}/btree/bt_split.c
        ${MICROPY_LIB_BERKELEY_DIR}/btree/bt_utils.c
        ${MICROPY_LIB_BERKELEY_DIR}/mpool/mpool.c
    )

    target_include_directories(micropy_extmod_btree PRIVATE
        ${MICROPY_LIB_BERKELEY_DIR}/PORT/include
    )

    target_compile_definitions(micropy_extmod_btree PRIVATE
        __DBINTERFACE_PRIVATE=1
        mpool_error=printf
        abort=abort_
        "virt_fd_t=void*"
    )

    # The include directories and compile definitions below are needed to build
    # modbtree.c and should be added to the main MicroPython target.

    list(APPEND MICROPY_INC_CORE
        "${MICROPY_LIB_BERKELEY_DIR}/PORT/include"
    )

    list(APPEND MICROPY_DEF_CORE
        __DBINTERFACE_PRIVATE=1
        "virt_fd_t=void*"
    )
endif()

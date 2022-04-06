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
    ${MICROPY_EXTMOD_DIR}/machine_pwm.c
    ${MICROPY_EXTMOD_DIR}/machine_signal.c
    ${MICROPY_EXTMOD_DIR}/machine_spi.c
    ${MICROPY_EXTMOD_DIR}/modbluetooth.c
    ${MICROPY_EXTMOD_DIR}/modbtree.c
    ${MICROPY_EXTMOD_DIR}/modframebuf.c
    ${MICROPY_EXTMOD_DIR}/modnetwork.c
    ${MICROPY_EXTMOD_DIR}/modonewire.c
    ${MICROPY_EXTMOD_DIR}/moduasyncio.c
    ${MICROPY_EXTMOD_DIR}/modubinascii.c
    ${MICROPY_EXTMOD_DIR}/moducryptolib.c
    ${MICROPY_EXTMOD_DIR}/moductypes.c
    ${MICROPY_EXTMOD_DIR}/moduhashlib.c
    ${MICROPY_EXTMOD_DIR}/moduheapq.c
    ${MICROPY_EXTMOD_DIR}/modujson.c
    ${MICROPY_EXTMOD_DIR}/moduos.c
    ${MICROPY_EXTMOD_DIR}/moduplatform.c
    ${MICROPY_EXTMOD_DIR}/modurandom.c
    ${MICROPY_EXTMOD_DIR}/modure.c
    ${MICROPY_EXTMOD_DIR}/moduselect.c
    ${MICROPY_EXTMOD_DIR}/modusocket.c
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

# Library for mbedtls

set(MICROPY_LIB_MBEDTLS_DIR "${MICROPY_DIR}/lib/mbedtls")

if(EXISTS "${MICROPY_LIB_MBEDTLS_DIR}/library/aes.c")
    add_library(micropy_lib_mbedtls INTERFACE)

    target_include_directories(micropy_lib_mbedtls INTERFACE
        ${MICROPY_LIB_MBEDTLS_DIR}/include
    )

    target_sources(micropy_lib_mbedtls INTERFACE
        ${MICROPY_LIB_MBEDTLS_DIR}/library/aes.c
        ${MICROPY_LIB_MBEDTLS_DIR}/library/aesni.c
        ${MICROPY_LIB_MBEDTLS_DIR}/library/arc4.c
        ${MICROPY_LIB_MBEDTLS_DIR}/library/asn1parse.c
        ${MICROPY_LIB_MBEDTLS_DIR}/library/asn1write.c
        ${MICROPY_LIB_MBEDTLS_DIR}/library/base64.c
        ${MICROPY_LIB_MBEDTLS_DIR}/library/bignum.c
        ${MICROPY_LIB_MBEDTLS_DIR}/library/blowfish.c
        ${MICROPY_LIB_MBEDTLS_DIR}/library/camellia.c
        ${MICROPY_LIB_MBEDTLS_DIR}/library/ccm.c
        ${MICROPY_LIB_MBEDTLS_DIR}/library/certs.c
        ${MICROPY_LIB_MBEDTLS_DIR}/library/chacha20.c
        ${MICROPY_LIB_MBEDTLS_DIR}/library/chachapoly.c
        ${MICROPY_LIB_MBEDTLS_DIR}/library/cipher.c
        ${MICROPY_LIB_MBEDTLS_DIR}/library/cipher_wrap.c
        ${MICROPY_LIB_MBEDTLS_DIR}/library/cmac.c
        ${MICROPY_LIB_MBEDTLS_DIR}/library/ctr_drbg.c
        ${MICROPY_LIB_MBEDTLS_DIR}/library/debug.c
        ${MICROPY_LIB_MBEDTLS_DIR}/library/des.c
        ${MICROPY_LIB_MBEDTLS_DIR}/library/dhm.c
        ${MICROPY_LIB_MBEDTLS_DIR}/library/ecdh.c
        ${MICROPY_LIB_MBEDTLS_DIR}/library/ecdsa.c
        ${MICROPY_LIB_MBEDTLS_DIR}/library/ecjpake.c
        ${MICROPY_LIB_MBEDTLS_DIR}/library/ecp.c
        ${MICROPY_LIB_MBEDTLS_DIR}/library/ecp_curves.c
        ${MICROPY_LIB_MBEDTLS_DIR}/library/entropy.c
        ${MICROPY_LIB_MBEDTLS_DIR}/library/entropy_poll.c
        ${MICROPY_LIB_MBEDTLS_DIR}/library/error.c
        ${MICROPY_LIB_MBEDTLS_DIR}/library/gcm.c
        ${MICROPY_LIB_MBEDTLS_DIR}/library/havege.c
        ${MICROPY_LIB_MBEDTLS_DIR}/library/hmac_drbg.c
        ${MICROPY_LIB_MBEDTLS_DIR}/library/md2.c
        ${MICROPY_LIB_MBEDTLS_DIR}/library/md4.c
        ${MICROPY_LIB_MBEDTLS_DIR}/library/md5.c
        ${MICROPY_LIB_MBEDTLS_DIR}/library/md.c
        ${MICROPY_LIB_MBEDTLS_DIR}/library/md_wrap.c
        ${MICROPY_LIB_MBEDTLS_DIR}/library/oid.c
        ${MICROPY_LIB_MBEDTLS_DIR}/library/padlock.c
        ${MICROPY_LIB_MBEDTLS_DIR}/library/pem.c
        ${MICROPY_LIB_MBEDTLS_DIR}/library/pk.c
        ${MICROPY_LIB_MBEDTLS_DIR}/library/pkcs11.c
        ${MICROPY_LIB_MBEDTLS_DIR}/library/pkcs12.c
        ${MICROPY_LIB_MBEDTLS_DIR}/library/pkcs5.c
        ${MICROPY_LIB_MBEDTLS_DIR}/library/pkparse.c
        ${MICROPY_LIB_MBEDTLS_DIR}/library/pk_wrap.c
        ${MICROPY_LIB_MBEDTLS_DIR}/library/pkwrite.c
        ${MICROPY_LIB_MBEDTLS_DIR}/library/platform.c
        ${MICROPY_LIB_MBEDTLS_DIR}/library/platform_util.c
        ${MICROPY_LIB_MBEDTLS_DIR}/library/poly1305.c
        ${MICROPY_LIB_MBEDTLS_DIR}/library/ripemd160.c
        ${MICROPY_LIB_MBEDTLS_DIR}/library/rsa.c
        ${MICROPY_LIB_MBEDTLS_DIR}/library/rsa_internal.c
        ${MICROPY_LIB_MBEDTLS_DIR}/library/sha1.c
        ${MICROPY_LIB_MBEDTLS_DIR}/library/sha256.c
        ${MICROPY_LIB_MBEDTLS_DIR}/library/sha512.c
        ${MICROPY_LIB_MBEDTLS_DIR}/library/ssl_cache.c
        ${MICROPY_LIB_MBEDTLS_DIR}/library/ssl_ciphersuites.c
        ${MICROPY_LIB_MBEDTLS_DIR}/library/ssl_cli.c
        ${MICROPY_LIB_MBEDTLS_DIR}/library/ssl_cookie.c
        ${MICROPY_LIB_MBEDTLS_DIR}/library/ssl_srv.c
        ${MICROPY_LIB_MBEDTLS_DIR}/library/ssl_ticket.c
        ${MICROPY_LIB_MBEDTLS_DIR}/library/ssl_tls.c
        ${MICROPY_LIB_MBEDTLS_DIR}/library/timing.c
        ${MICROPY_LIB_MBEDTLS_DIR}/library/x509.c
        ${MICROPY_LIB_MBEDTLS_DIR}/library/x509_create.c
        ${MICROPY_LIB_MBEDTLS_DIR}/library/x509_crl.c
        ${MICROPY_LIB_MBEDTLS_DIR}/library/x509_crt.c
        ${MICROPY_LIB_MBEDTLS_DIR}/library/x509_csr.c
        ${MICROPY_LIB_MBEDTLS_DIR}/library/x509write_crt.c
        ${MICROPY_LIB_MBEDTLS_DIR}/library/x509write_csr.c
        ${MICROPY_LIB_MBEDTLS_DIR}/library/xtea.c
    )

    target_compile_definitions(micropy_lib_mbedtls INTERFACE
        MBEDTLS_CONFIG_FILE="ports/stm32/mbedtls/mbedtls_config.h"
    )

    list(APPEND MICROPY_INC_CORE
        "${MICROPY_LIB_MBEDTLS_DIR}/include"
    )
endif()

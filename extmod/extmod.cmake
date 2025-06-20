# CMake fragment for MicroPython extmod component

set(MICROPY_EXTMOD_DIR "${MICROPY_DIR}/extmod")
set(MICROPY_OOFATFS_DIR "${MICROPY_DIR}/lib/oofatfs")

set(MICROPY_SOURCE_EXTMOD
    ${MICROPY_DIR}/shared/libc/abort_.c
    ${MICROPY_DIR}/shared/libc/printf.c
    ${MICROPY_EXTMOD_DIR}/btstack/modbluetooth_btstack.c
    ${MICROPY_EXTMOD_DIR}/machine_adc.c
    ${MICROPY_EXTMOD_DIR}/machine_adc_block.c
    ${MICROPY_EXTMOD_DIR}/machine_bitstream.c
    ${MICROPY_EXTMOD_DIR}/machine_i2c.c
    ${MICROPY_EXTMOD_DIR}/machine_i2s.c
    ${MICROPY_EXTMOD_DIR}/machine_mem.c
    ${MICROPY_EXTMOD_DIR}/machine_pulse.c
    ${MICROPY_EXTMOD_DIR}/machine_pwm.c
    ${MICROPY_EXTMOD_DIR}/machine_signal.c
    ${MICROPY_EXTMOD_DIR}/machine_spi.c
    ${MICROPY_EXTMOD_DIR}/machine_uart.c
    ${MICROPY_EXTMOD_DIR}/machine_usb_device.c
    ${MICROPY_EXTMOD_DIR}/machine_wdt.c
    ${MICROPY_EXTMOD_DIR}/modbluetooth.c
    ${MICROPY_EXTMOD_DIR}/modframebuf.c
    ${MICROPY_EXTMOD_DIR}/modlwip.c
    ${MICROPY_EXTMOD_DIR}/modmachine.c
    ${MICROPY_EXTMOD_DIR}/modnetwork.c
    ${MICROPY_EXTMOD_DIR}/modonewire.c
    ${MICROPY_EXTMOD_DIR}/modasyncio.c
    ${MICROPY_EXTMOD_DIR}/modbinascii.c
    ${MICROPY_EXTMOD_DIR}/modcryptolib.c
    ${MICROPY_EXTMOD_DIR}/moductypes.c
    ${MICROPY_EXTMOD_DIR}/moddeflate.c
    ${MICROPY_EXTMOD_DIR}/modhashlib.c
    ${MICROPY_EXTMOD_DIR}/modheapq.c
    ${MICROPY_EXTMOD_DIR}/modjson.c
    ${MICROPY_EXTMOD_DIR}/modos.c
    ${MICROPY_EXTMOD_DIR}/modplatform.c
    ${MICROPY_EXTMOD_DIR}/modrandom.c
    ${MICROPY_EXTMOD_DIR}/modre.c
    ${MICROPY_EXTMOD_DIR}/modselect.c
    ${MICROPY_EXTMOD_DIR}/modsocket.c
    ${MICROPY_EXTMOD_DIR}/modtls_axtls.c
    ${MICROPY_EXTMOD_DIR}/modtls_mbedtls.c
    ${MICROPY_EXTMOD_DIR}/modtime.c
    ${MICROPY_EXTMOD_DIR}/modvfs.c
    ${MICROPY_EXTMOD_DIR}/modwebsocket.c
    ${MICROPY_EXTMOD_DIR}/modwebrepl.c
    ${MICROPY_EXTMOD_DIR}/network_cyw43.c
    ${MICROPY_EXTMOD_DIR}/network_lwip.c
    ${MICROPY_EXTMOD_DIR}/network_ninaw10.c
    ${MICROPY_EXTMOD_DIR}/network_wiznet5k.c
    ${MICROPY_EXTMOD_DIR}/os_dupterm.c
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

# Single-precision libm math library.

set(MICROPY_SOURCE_LIB_LIBM
    ${MICROPY_DIR}/lib/libm/acoshf.c
    ${MICROPY_DIR}/lib/libm/asinfacosf.c
    ${MICROPY_DIR}/lib/libm/asinhf.c
    ${MICROPY_DIR}/lib/libm/atan2f.c
    ${MICROPY_DIR}/lib/libm/atanf.c
    ${MICROPY_DIR}/lib/libm/atanhf.c
    ${MICROPY_DIR}/lib/libm/ef_rem_pio2.c
    ${MICROPY_DIR}/lib/libm/erf_lgamma.c
    ${MICROPY_DIR}/lib/libm/fmodf.c
    ${MICROPY_DIR}/lib/libm/kf_cos.c
    ${MICROPY_DIR}/lib/libm/kf_rem_pio2.c
    ${MICROPY_DIR}/lib/libm/kf_sin.c
    ${MICROPY_DIR}/lib/libm/kf_tan.c
    ${MICROPY_DIR}/lib/libm/log1pf.c
    ${MICROPY_DIR}/lib/libm/math.c
    ${MICROPY_DIR}/lib/libm/nearbyintf.c
    ${MICROPY_DIR}/lib/libm/roundf.c
    ${MICROPY_DIR}/lib/libm/sf_cos.c
    ${MICROPY_DIR}/lib/libm/sf_erf.c
    ${MICROPY_DIR}/lib/libm/sf_frexp.c
    ${MICROPY_DIR}/lib/libm/sf_ldexp.c
    ${MICROPY_DIR}/lib/libm/sf_modf.c
    ${MICROPY_DIR}/lib/libm/sf_sin.c
    ${MICROPY_DIR}/lib/libm/sf_tan.c
    ${MICROPY_DIR}/lib/libm/wf_lgamma.c
    ${MICROPY_DIR}/lib/libm/wf_tgamma.c
)

# Choose only one of these sqrt implementations, software or hardware.
set(MICROPY_SOURCE_LIB_LIBM_SQRT_SW ${MICROPY_DIR}/lib/libm/ef_sqrt.c)
set(MICROPY_SOURCE_LIB_LIBM_SQRT_HW ${MICROPY_DIR}/lib/libm/thumb_vfp_sqrtf.c)

# Library for btree module and associated code

if(MICROPY_PY_BTREE)
    set(MICROPY_LIB_BERKELEY_DIR "${MICROPY_DIR}/lib/berkeley-db-1.xx")
    string(CONCAT GIT_SUBMODULES "${GIT_SUBMODULES} " lib/berkeley-db-1.xx)

    if(ECHO_SUBMODULES)
        # No-op, we're just doing submodule/variant discovery.
        # Cannot run the add_library/target_include_directories rules (even though
        # the build won't run) because IDF will attempt verify the files exist.
    elseif(NOT EXISTS ${MICROPY_LIB_BERKELEY_DIR}/README)
        # Regular build, submodule not initialised -- fail with a clear error.
        message(FATAL_ERROR " MICROPY_PY_BTREE is enabled but the berkeley-db submodule is not initialised.\n Run 'make BOARD=${MICROPY_BOARD} submodules'")
    else()
        # Regular build, we have the submodule.
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
            ${MICROPY_LIB_BERKELEY_DIR}/include
        )

        if(NOT BERKELEY_DB_CONFIG_FILE)
            set(BERKELEY_DB_CONFIG_FILE "${MICROPY_DIR}/extmod/berkeley-db/berkeley_db_config_port.h")
        endif()

        target_compile_definitions(micropy_extmod_btree PRIVATE
            BERKELEY_DB_CONFIG_FILE="${BERKELEY_DB_CONFIG_FILE}"
        )

        # The include directories and compile definitions below are needed to build
        # modbtree.c and should be added to the main MicroPython target.

        list(APPEND MICROPY_INC_CORE
            "${MICROPY_LIB_BERKELEY_DIR}/include"
        )

        list(APPEND MICROPY_DEF_CORE
            MICROPY_PY_BTREE=1
            BERKELEY_DB_CONFIG_FILE="${BERKELEY_DB_CONFIG_FILE}"
        )

        list(APPEND MICROPY_SOURCE_EXTMOD
            ${MICROPY_EXTMOD_DIR}/modbtree.c
        )
    endif()
endif()

# Library for mbedtls

if(MICROPY_SSL_MBEDTLS)
    add_library(micropy_lib_mbedtls INTERFACE)

    set(MICROPY_LIB_MBEDTLS_DIR "${MICROPY_DIR}/lib/mbedtls")

    target_include_directories(micropy_lib_mbedtls INTERFACE
        ${MICROPY_LIB_MBEDTLS_DIR}/include
    )

    target_sources(micropy_lib_mbedtls INTERFACE
        ${MICROPY_DIR}/lib/mbedtls_errors/mp_mbedtls_errors.c
        ${MICROPY_LIB_MBEDTLS_DIR}/library/aes.c
        ${MICROPY_LIB_MBEDTLS_DIR}/library/aesni.c
        ${MICROPY_LIB_MBEDTLS_DIR}/library/asn1parse.c
        ${MICROPY_LIB_MBEDTLS_DIR}/library/asn1write.c
        ${MICROPY_LIB_MBEDTLS_DIR}/library/base64.c
        ${MICROPY_LIB_MBEDTLS_DIR}/library/bignum_core.c
        ${MICROPY_LIB_MBEDTLS_DIR}/library/bignum_mod.c
        ${MICROPY_LIB_MBEDTLS_DIR}/library/bignum_mod_raw.c
        ${MICROPY_LIB_MBEDTLS_DIR}/library/bignum.c
        ${MICROPY_LIB_MBEDTLS_DIR}/library/camellia.c
        ${MICROPY_LIB_MBEDTLS_DIR}/library/ccm.c
        ${MICROPY_LIB_MBEDTLS_DIR}/library/chacha20.c
        ${MICROPY_LIB_MBEDTLS_DIR}/library/chachapoly.c
        ${MICROPY_LIB_MBEDTLS_DIR}/library/cipher.c
        ${MICROPY_LIB_MBEDTLS_DIR}/library/cipher_wrap.c
        ${MICROPY_LIB_MBEDTLS_DIR}/library/nist_kw.c
        ${MICROPY_LIB_MBEDTLS_DIR}/library/aria.c
        ${MICROPY_LIB_MBEDTLS_DIR}/library/cmac.c
        ${MICROPY_LIB_MBEDTLS_DIR}/library/mps_reader.c
        ${MICROPY_LIB_MBEDTLS_DIR}/library/mps_trace.c
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
        ${MICROPY_LIB_MBEDTLS_DIR}/library/gcm.c
        ${MICROPY_LIB_MBEDTLS_DIR}/library/hmac_drbg.c
        ${MICROPY_LIB_MBEDTLS_DIR}/library/md5.c
        ${MICROPY_LIB_MBEDTLS_DIR}/library/md.c
        ${MICROPY_LIB_MBEDTLS_DIR}/library/oid.c
        ${MICROPY_LIB_MBEDTLS_DIR}/library/padlock.c
        ${MICROPY_LIB_MBEDTLS_DIR}/library/pem.c
        ${MICROPY_LIB_MBEDTLS_DIR}/library/pk.c
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
        ${MICROPY_LIB_MBEDTLS_DIR}/library/rsa_alt_helpers.c
        ${MICROPY_LIB_MBEDTLS_DIR}/library/sha1.c
        ${MICROPY_LIB_MBEDTLS_DIR}/library/sha256.c
        ${MICROPY_LIB_MBEDTLS_DIR}/library/sha512.c
        ${MICROPY_LIB_MBEDTLS_DIR}/library/ssl_cache.c
        ${MICROPY_LIB_MBEDTLS_DIR}/library/ssl_ciphersuites.c
        ${MICROPY_LIB_MBEDTLS_DIR}/library/ssl_tls12_client.c
        ${MICROPY_LIB_MBEDTLS_DIR}/library/ssl_tls12_server.c
        ${MICROPY_LIB_MBEDTLS_DIR}/library/ssl_client.c
        ${MICROPY_LIB_MBEDTLS_DIR}/library/ssl_cookie.c
        ${MICROPY_LIB_MBEDTLS_DIR}/library/ssl_debug_helpers_generated.c
        ${MICROPY_LIB_MBEDTLS_DIR}/library/ssl_msg.c
        ${MICROPY_LIB_MBEDTLS_DIR}/library/ssl_ticket.c
        ${MICROPY_LIB_MBEDTLS_DIR}/library/ssl_tls.c
        ${MICROPY_LIB_MBEDTLS_DIR}/library/timing.c
        ${MICROPY_LIB_MBEDTLS_DIR}/library/constant_time.c
        ${MICROPY_LIB_MBEDTLS_DIR}/library/x509.c
        ${MICROPY_LIB_MBEDTLS_DIR}/library/x509_create.c
        ${MICROPY_LIB_MBEDTLS_DIR}/library/x509_crl.c
        ${MICROPY_LIB_MBEDTLS_DIR}/library/x509_crt.c
        ${MICROPY_LIB_MBEDTLS_DIR}/library/x509_csr.c
        ${MICROPY_LIB_MBEDTLS_DIR}/library/x509write_crt.c
        ${MICROPY_LIB_MBEDTLS_DIR}/library/x509write_csr.c
    )

    if(NOT MBEDTLS_CONFIG_FILE)
        set(MBEDTLS_CONFIG_FILE "${MICROPY_PORT_DIR}/mbedtls/mbedtls_config_port.h")
    endif()

    target_compile_definitions(micropy_lib_mbedtls INTERFACE
        MBEDTLS_CONFIG_FILE="${MBEDTLS_CONFIG_FILE}"
    )

    list(APPEND MICROPY_INC_CORE
        "${MICROPY_LIB_MBEDTLS_DIR}/include"
    )
endif()

# Library for lwIP network stack

if(MICROPY_PY_LWIP)
    add_library(micropy_lib_lwip INTERFACE)

    set(MICROPY_LIB_LWIP_DIR "${MICROPY_DIR}/lib/lwip/src")

    target_include_directories(micropy_lib_lwip INTERFACE
        ${MICROPY_LIB_LWIP_DIR}/include
    )

    target_sources(micropy_lib_lwip INTERFACE
        ${MICROPY_DIR}/shared/netutils/netutils.c
        ${MICROPY_LIB_LWIP_DIR}/apps/mdns/mdns.c
        ${MICROPY_LIB_LWIP_DIR}/core/def.c
        ${MICROPY_LIB_LWIP_DIR}/core/dns.c
        ${MICROPY_LIB_LWIP_DIR}/core/inet_chksum.c
        ${MICROPY_LIB_LWIP_DIR}/core/init.c
        ${MICROPY_LIB_LWIP_DIR}/core/ip.c
        ${MICROPY_LIB_LWIP_DIR}/core/ipv4/autoip.c
        ${MICROPY_LIB_LWIP_DIR}/core/ipv4/dhcp.c
        ${MICROPY_LIB_LWIP_DIR}/core/ipv4/etharp.c
        ${MICROPY_LIB_LWIP_DIR}/core/ipv4/icmp.c
        ${MICROPY_LIB_LWIP_DIR}/core/ipv4/igmp.c
        ${MICROPY_LIB_LWIP_DIR}/core/ipv4/ip4.c
        ${MICROPY_LIB_LWIP_DIR}/core/ipv4/ip4_addr.c
        ${MICROPY_LIB_LWIP_DIR}/core/ipv4/ip4_frag.c
        ${MICROPY_LIB_LWIP_DIR}/core/ipv6/dhcp6.c
        ${MICROPY_LIB_LWIP_DIR}/core/ipv6/ethip6.c
        ${MICROPY_LIB_LWIP_DIR}/core/ipv6/icmp6.c
        ${MICROPY_LIB_LWIP_DIR}/core/ipv6/inet6.c
        ${MICROPY_LIB_LWIP_DIR}/core/ipv6/ip6.c
        ${MICROPY_LIB_LWIP_DIR}/core/ipv6/ip6_addr.c
        ${MICROPY_LIB_LWIP_DIR}/core/ipv6/ip6_frag.c
        ${MICROPY_LIB_LWIP_DIR}/core/ipv6/mld6.c
        ${MICROPY_LIB_LWIP_DIR}/core/ipv6/nd6.c
        ${MICROPY_LIB_LWIP_DIR}/core/mem.c
        ${MICROPY_LIB_LWIP_DIR}/core/memp.c
        ${MICROPY_LIB_LWIP_DIR}/core/netif.c
        ${MICROPY_LIB_LWIP_DIR}/core/pbuf.c
        ${MICROPY_LIB_LWIP_DIR}/core/raw.c
        ${MICROPY_LIB_LWIP_DIR}/core/stats.c
        ${MICROPY_LIB_LWIP_DIR}/core/sys.c
        ${MICROPY_LIB_LWIP_DIR}/core/tcp.c
        ${MICROPY_LIB_LWIP_DIR}/core/tcp_in.c
        ${MICROPY_LIB_LWIP_DIR}/core/tcp_out.c
        ${MICROPY_LIB_LWIP_DIR}/core/timeouts.c
        ${MICROPY_LIB_LWIP_DIR}/core/udp.c
        ${MICROPY_LIB_LWIP_DIR}/netif/ethernet.c
    )

    list(APPEND MICROPY_INC_CORE
        ${MICROPY_LIB_LWIP_DIR}/include
    )

    string(CONCAT GIT_SUBMODULES "${GIT_SUBMODULES} " lib/lwip)
endif()

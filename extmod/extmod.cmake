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
    ${MICROPY_EXTMOD_DIR}/machine_can.c
    ${MICROPY_EXTMOD_DIR}/machine_i2c.c
    ${MICROPY_EXTMOD_DIR}/machine_i2c_target.c
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
    ${MICROPY_EXTMOD_DIR}/modmarshal.c
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
    ${MICROPY_EXTMOD_DIR}/network_ppp_lwip.c
    ${MICROPY_EXTMOD_DIR}/network_usbd_ncm.c
    ${MICROPY_EXTMOD_DIR}/network_wiznet5k.c
    ${MICROPY_EXTMOD_DIR}/os_dupterm.c
    ${MICROPY_EXTMOD_DIR}/vfs.c
    ${MICROPY_EXTMOD_DIR}/vfs_blockdev.c
    ${MICROPY_EXTMOD_DIR}/vfs_fat.c
    ${MICROPY_EXTMOD_DIR}/vfs_fat_diskio.c
    ${MICROPY_EXTMOD_DIR}/vfs_fat_file.c
    ${MICROPY_EXTMOD_DIR}/vfs_lfs.c
    ${MICROPY_EXTMOD_DIR}/vfs_rom.c
    ${MICROPY_EXTMOD_DIR}/vfs_rom_file.c
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
    list(APPEND GIT_SUBMODULES lib/berkeley-db-1.xx)

    if(NOT UPDATE_SUBMODULES AND NOT EXISTS ${MICROPY_LIB_BERKELEY_DIR}/README)
        # Regular build, submodule not initialised -- fail with a clear error.
        message(FATAL_ERROR " MICROPY_PY_BTREE is enabled but the berkeley-db submodule is not initialised.\n Run 'make BOARD=${MICROPY_BOARD} submodules'")
    endif()

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

    # We need to suppress certain warnings to get berkeley-db to compile cleanly.
    # Should be kept in sync with extmod.mk.
    target_compile_options(micropy_extmod_btree PRIVATE
        -Wno-old-style-definition -Wno-sign-compare -Wno-unused-parameter -Wno-deprecated-non-prototype -Wno-unknown-warning-option
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

# Library for mbedtls

if(MICROPY_SSL_MBEDTLS)
    add_library(micropy_lib_mbedtls INTERFACE)

    if(NOT MBEDTLS_CONFIG_FILE)
        # Note: Because mbedTLS CMakeLists.txt sets these as cache variables, we have to set them as cache variables as well or the value
        # will be overridden with the default (this changed in CMake 3.21 but we can't bump the minimum CMake version without breaking other things...)
        set(MBEDTLS_CONFIG_FILE "${MICROPY_PORT_DIR}/mbedtls/mbedtls_config_port.h" CACHE FILEPATH "Mbed TLS config file (MicroPython override).")
    endif()
    if(NOT TF_PSA_CRYPTO_CONFIG_FILE)
        set(TF_PSA_CRYPTO_CONFIG_FILE "${MICROPY_PORT_DIR}/mbedtls/crypto_config_port.h" CACHE FILEPATH "Mbed TLS config file (MicroPython override).")
    endif()

    set(ENABLE_TESTING OFF CACHE BOOL "mbedtls: enable testing")
    set(ENABLE_PROGRAMS OFF CACHE BOOL "mbedtls: enable programs")

    add_subdirectory("${MICROPY_DIR}/lib/mbedtls" "${CMAKE_CURRENT_BINARY_DIR}/mbedtls")

    target_link_libraries(micropy_lib_mbedtls INTERFACE mbedtls)

    # Need to inject MicroPython directory so mbedTLS config include can find extmod/ directory, board-specific files, etc.
    #
    # The list of libraries in the foreach includes all the sub-libraries under tfpsacrypto. I couldn't find a way
    # to get this list using CMake, so copied Zephyr's hard-coded list from main/modules/mbedtls/CMakeLists.txt
    # in Zephyr...
    foreach(mbedtls_lib IN ITEMS mbedtls mbedx509 tfpsacrypto builtin p256-m everest pqcp extras platform utilities)
        target_include_directories(${mbedtls_lib} PRIVATE
           "${MICROPY_DIR}"
           "${MICROPY_PORT_DIR}"
           "${MICROPY_BOARD_DIR}")
    endforeach()

    # QSTR generation needs the mbedTLS library include directories
    # (as some QSTR selections depend on mbedTlS config)
    get_target_property(MBEDTLS_INC_DIRS mbedtls INTERFACE_INCLUDE_DIRECTORIES)
    list(FILTER MBEDTLS_INC_DIRS EXCLUDE REGEX "\\$<INSTALL_INTERFACE:")  # Only build include dirs, not install ones
    list(APPEND MICROPY_CPP_INC_EXTRA ${MBEDTLS_INC_DIRS})

    # Use linker wrap to have the patched mbedtls_strerror from mp_mbedtls_strerror
    # replace the version built by mbedtls itself
    list(APPEND MICROPY_SOURCE_EXTMOD "${MICROPY_DIR}/lib/mbedtls_errors/mp_mbedtls_errors.c")
    target_link_options(mbedtls PUBLIC -Wl,--wrap=mbedtls_strerror)
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
        ${MICROPY_LIB_LWIP_DIR}/apps/mdns/mdns_domain.c
        ${MICROPY_LIB_LWIP_DIR}/apps/mdns/mdns_out.c
        ${MICROPY_LIB_LWIP_DIR}/core/def.c
        ${MICROPY_LIB_LWIP_DIR}/core/dns.c
        ${MICROPY_LIB_LWIP_DIR}/core/inet_chksum.c
        ${MICROPY_LIB_LWIP_DIR}/core/init.c
        ${MICROPY_LIB_LWIP_DIR}/core/ip.c
        ${MICROPY_LIB_LWIP_DIR}/core/ipv4/acd.c
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
        ${MICROPY_LIB_LWIP_DIR}/netif/ppp/auth.c
        ${MICROPY_LIB_LWIP_DIR}/netif/ppp/ccp.c
        ${MICROPY_LIB_LWIP_DIR}/netif/ppp/chap-md5.c
        ${MICROPY_LIB_LWIP_DIR}/netif/ppp/chap_ms.c
        ${MICROPY_LIB_LWIP_DIR}/netif/ppp/chap-new.c
        ${MICROPY_LIB_LWIP_DIR}/netif/ppp/demand.c
        ${MICROPY_LIB_LWIP_DIR}/netif/ppp/eap.c
        ${MICROPY_LIB_LWIP_DIR}/netif/ppp/ecp.c
        ${MICROPY_LIB_LWIP_DIR}/netif/ppp/eui64.c
        ${MICROPY_LIB_LWIP_DIR}/netif/ppp/fsm.c
        ${MICROPY_LIB_LWIP_DIR}/netif/ppp/ipcp.c
        ${MICROPY_LIB_LWIP_DIR}/netif/ppp/ipv6cp.c
        ${MICROPY_LIB_LWIP_DIR}/netif/ppp/lcp.c
        ${MICROPY_LIB_LWIP_DIR}/netif/ppp/magic.c
        ${MICROPY_LIB_LWIP_DIR}/netif/ppp/mppe.c
        ${MICROPY_LIB_LWIP_DIR}/netif/ppp/multilink.c
        ${MICROPY_LIB_LWIP_DIR}/netif/ppp/polarssl/md5.c
        ${MICROPY_LIB_LWIP_DIR}/netif/ppp/pppapi.c
        ${MICROPY_LIB_LWIP_DIR}/netif/ppp/ppp.c
        ${MICROPY_LIB_LWIP_DIR}/netif/ppp/pppcrypt.c
        ${MICROPY_LIB_LWIP_DIR}/netif/ppp/pppoe.c
        ${MICROPY_LIB_LWIP_DIR}/netif/ppp/pppol2tp.c
        ${MICROPY_LIB_LWIP_DIR}/netif/ppp/pppos.c
        ${MICROPY_LIB_LWIP_DIR}/netif/ppp/upap.c
        ${MICROPY_LIB_LWIP_DIR}/netif/ppp/utils.c
        ${MICROPY_LIB_LWIP_DIR}/netif/ppp/vj.c
    )

    list(APPEND MICROPY_INC_CORE
        ${MICROPY_LIB_LWIP_DIR}/include
    )

    list(APPEND GIT_SUBMODULES lib/lwip)
endif()

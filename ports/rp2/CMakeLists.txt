cmake_minimum_required(VERSION 3.13)

# Set build type to reduce firmware size
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE MinSizeRel)
endif()

# Set main target and component locations
set(MICROPY_TARGET firmware)
get_filename_component(MICROPY_DIR "../.." ABSOLUTE)
if (PICO_SDK_PATH_OVERRIDE)
    set(PICO_SDK_PATH ${PICO_SDK_PATH_OVERRIDE})
else()
    set(PICO_SDK_PATH ../../lib/pico-sdk)
endif()

# Use the local tinyusb instead of the one in pico-sdk
set(PICO_TINYUSB_PATH ${MICROPY_DIR}/lib/tinyusb)
# Use the local lwip instead of the one in pico-sdk
set(PICO_LWIP_PATH ${MICROPY_DIR}/lib/lwip)
# Use the local btstack instead of the one in pico-sdk
set(PICO_BTSTACK_PATH ${MICROPY_DIR}/lib/btstack)

# Set the location of this port's directory.
set(MICROPY_PORT_DIR ${CMAKE_CURRENT_LIST_DIR})

# Set the board if it's not already set.
if(NOT MICROPY_BOARD)
    set(MICROPY_BOARD RPI_PICO)
endif()

# Set the board directory and check that it exists.
if(NOT MICROPY_BOARD_DIR)
    set(MICROPY_BOARD_DIR ${MICROPY_PORT_DIR}/boards/${MICROPY_BOARD})
endif()
get_filename_component(MICROPY_BOARD_DIR ${MICROPY_BOARD_DIR} ABSOLUTE)
if(NOT EXISTS ${MICROPY_BOARD_DIR}/mpconfigboard.cmake)
    message(FATAL_ERROR "Invalid MICROPY_BOARD specified: ${MICROPY_BOARD}")
endif()

# If a board variant is specified, check that it exists.
if(MICROPY_BOARD_VARIANT)
    if(NOT EXISTS ${MICROPY_BOARD_DIR}/mpconfigvariant_${MICROPY_BOARD_VARIANT}.cmake)
        message(FATAL_ERROR "Invalid MICROPY_BOARD_VARIANT specified: ${MICROPY_BOARD_VARIANT}")
    endif()
endif()

set(MICROPY_USER_FROZEN_MANIFEST ${MICROPY_FROZEN_MANIFEST})

# Include board config, it may override MICROPY_FROZEN_MANIFEST
include(${MICROPY_BOARD_DIR}/mpconfigboard.cmake)
if(NOT MICROPY_BOARD_VARIANT)
    include(${MICROPY_BOARD_DIR}/mpconfigvariant.cmake OPTIONAL)
else()
    include(${MICROPY_BOARD_DIR}/mpconfigvariant_${MICROPY_BOARD_VARIANT}.cmake)
endif()

# Set the PICO_BOARD if it's not already set (allow a board to override it).
if(NOT PICO_BOARD)
    string(TOLOWER ${MICROPY_BOARD} PICO_BOARD)
endif()

# Set the amount of C heap, if it's not already set.
# If a board uses malloc then it must set this to at least 4096.
if(NOT MICROPY_C_HEAP_SIZE)
    set(MICROPY_C_HEAP_SIZE 0)
endif()

# Enable extmod components that will be configured by extmod.cmake.
# A board may also have enabled additional components.
set(MICROPY_SSL_MBEDTLS ON)

# Use the local cyw43_driver instead of the one in pico-sdk
if (MICROPY_PY_NETWORK_CYW43)
    set(PICO_CYW43_DRIVER_PATH ${MICROPY_DIR}/lib/cyw43-driver)
endif()

# Necessary submodules for all boards.
string(CONCAT GIT_SUBMODULES "${GIT_SUBMODULES} " lib/mbedtls)
string(CONCAT GIT_SUBMODULES "${GIT_SUBMODULES} " lib/tinyusb)

# Include component cmake fragments
include(${MICROPY_DIR}/py/py.cmake)
include(${MICROPY_DIR}/extmod/extmod.cmake)
include(${PICO_SDK_PATH}/pico_sdk_init.cmake)

# Define the top-level project
project(${MICROPY_TARGET})

pico_sdk_init()

include(${MICROPY_DIR}/py/usermod.cmake)

add_executable(${MICROPY_TARGET})

# Provide a C-level definitions of PICO_ARM.
# (The pico-sdk already defines PICO_RISCV when it's enabled.)
if(PICO_ARM)
    target_compile_definitions(pico_platform_headers INTERFACE
        PICO_ARM=1
    )
endif()

set(MICROPY_QSTRDEFS_PORT
    ${MICROPY_PORT_DIR}/qstrdefsport.h
)

set(MICROPY_SOURCE_LIB
    ${MICROPY_DIR}/lib/littlefs/lfs1.c
    ${MICROPY_DIR}/lib/littlefs/lfs1_util.c
    ${MICROPY_DIR}/lib/littlefs/lfs2.c
    ${MICROPY_DIR}/lib/littlefs/lfs2_util.c
    ${MICROPY_DIR}/lib/oofatfs/ff.c
    ${MICROPY_DIR}/lib/oofatfs/ffunicode.c
    ${MICROPY_DIR}/shared/netutils/dhcpserver.c
    ${MICROPY_DIR}/shared/netutils/netutils.c
    ${MICROPY_DIR}/shared/netutils/trace.c
    ${MICROPY_DIR}/shared/readline/readline.c
    ${MICROPY_DIR}/shared/runtime/gchelper_native.c
    ${MICROPY_DIR}/shared/runtime/interrupt_char.c
    ${MICROPY_DIR}/shared/runtime/mpirq.c
    ${MICROPY_DIR}/shared/runtime/pyexec.c
    ${MICROPY_DIR}/shared/runtime/stdout_helpers.c
    ${MICROPY_DIR}/shared/runtime/softtimer.c
    ${MICROPY_DIR}/shared/runtime/sys_stdio_mphal.c
    ${MICROPY_DIR}/shared/timeutils/timeutils.c
    ${MICROPY_DIR}/shared/tinyusb/mp_usbd.c
    ${MICROPY_DIR}/shared/tinyusb/mp_usbd_cdc.c
    ${MICROPY_DIR}/shared/tinyusb/mp_usbd_descriptor.c
    ${MICROPY_DIR}/shared/tinyusb/mp_usbd_runtime.c
)

if(PICO_ARM)
    list(APPEND MICROPY_SOURCE_LIB
        ${MICROPY_DIR}/shared/runtime/gchelper_thumb1.s
    )
elseif(PICO_RISCV)
    list(APPEND MICROPY_SOURCE_LIB
        ${MICROPY_DIR}/shared/runtime/gchelper_rv32i.s
    )
endif()

set(MICROPY_SOURCE_DRIVERS
    ${MICROPY_DIR}/drivers/bus/softspi.c
    ${MICROPY_DIR}/drivers/dht/dht.c
)

set(MICROPY_SOURCE_PORT
    clocks_extra.c
    datetime_patch.c
    fatfs_port.c
    help.c
    machine_bitstream.c
    machine_i2c.c
    machine_pin.c
    machine_rtc.c
    machine_spi.c
    machine_timer.c
    main.c
    modrp2.c
    mphalport.c
    mpnetworkport.c
    mpthreadport.c
    mutex_extra.c
    pendsv.c
    rp2_flash.c
    rp2_pio.c
    rp2_dma.c
    uart.c
    usbd.c
    msc_disk.c
    mbedtls/mbedtls_port.c
    ${CMAKE_BINARY_DIR}/pins_${MICROPY_BOARD}.c
)

set(MICROPY_SOURCE_QSTR
    ${MICROPY_SOURCE_PY}
    ${MICROPY_DIR}/shared/readline/readline.c
    ${MICROPY_DIR}/shared/runtime/mpirq.c
    ${MICROPY_DIR}/shared/runtime/sys_stdio_mphal.c
    ${MICROPY_DIR}/shared/tinyusb/mp_usbd_runtime.c
    ${MICROPY_PORT_DIR}/machine_adc.c
    ${MICROPY_PORT_DIR}/machine_i2c.c
    ${MICROPY_PORT_DIR}/machine_pin.c
    ${MICROPY_PORT_DIR}/machine_rtc.c
    ${MICROPY_PORT_DIR}/machine_spi.c
    ${MICROPY_PORT_DIR}/machine_timer.c
    ${MICROPY_PORT_DIR}/machine_uart.c
    ${MICROPY_PORT_DIR}/machine_wdt.c
    ${MICROPY_PORT_DIR}/modrp2.c
    ${MICROPY_PORT_DIR}/modos.c
    ${MICROPY_PORT_DIR}/rp2_flash.c
    ${MICROPY_PORT_DIR}/rp2_pio.c
    ${MICROPY_PORT_DIR}/rp2_dma.c
    ${CMAKE_BINARY_DIR}/pins_${MICROPY_BOARD}.c
)

set(PICO_SDK_COMPONENTS
    boot_bootrom_headers
    hardware_adc
    hardware_base
    hardware_boot_lock
    hardware_clocks
    hardware_dma
    hardware_flash
    hardware_gpio
    hardware_i2c
    hardware_irq
    hardware_pio
    hardware_pll
    hardware_pwm
    hardware_regs
    hardware_resets
    hardware_spi
    hardware_structs
    hardware_sync
    hardware_sync_spin_lock
    hardware_timer
    hardware_uart
    hardware_watchdog
    hardware_xosc
    pico_aon_timer
    pico_base_headers
    pico_binary_info
    pico_bootrom
    pico_flash
    pico_multicore
    pico_platform
    pico_platform_compiler
    pico_platform_panic
    pico_platform_sections
    pico_runtime
    pico_runtime_init
    pico_stdio
    pico_stdlib
    pico_sync
    pico_time
    pico_unique_id
    pico_util
    tinyusb_common
    tinyusb_device
)

if(PICO_ARM)
    list(APPEND PICO_SDK_COMPONENTS
        cmsis_core
    )
elseif(PICO_RISCV)
    list(APPEND PICO_SDK_COMPONENTS
        hardware_hazard3
        hardware_riscv
    )
endif()

# Use our custom pico_float_micropython float implementation.  This is needed for two reasons:
# - to fix inf handling in pico-sdk's __wrap___aeabi_fadd();
# - so we can use our own libm functions, to fix inaccuracies in the pico-sdk versions.
pico_set_float_implementation(${MICROPY_TARGET} micropython)

# Define our custom pico_float_micropython component.
pico_add_library(pico_float_micropython)

# pico_float_micropython: add pico-sdk float and our libm source files.
target_sources(pico_float_micropython INTERFACE
    ${MICROPY_SOURCE_LIB_LIBM}
    ${MICROPY_SOURCE_LIB_LIBM_SQRT_SW}
    ${MICROPY_PORT_DIR}/libm_extra.c
)

if(PICO_RP2040)
    target_sources(pico_float_micropython INTERFACE
        ${PICO_SDK_PATH}/src/rp2_common/pico_float/float_aeabi_rp2040.S
        ${PICO_SDK_PATH}/src/rp2_common/pico_float/float_init_rom_rp2040.c
        ${PICO_SDK_PATH}/src/rp2_common/pico_float/float_v1_rom_shim_rp2040.S
    )
elseif(PICO_RP2350 AND PICO_ARM)
    target_sources(pico_float_micropython INTERFACE
        ${PICO_SDK_PATH}/src/rp2_common/pico_float/float_aeabi_dcp.S
        ${PICO_SDK_PATH}/src/rp2_common/pico_float/float_common_m33.S
    )
endif()

# pico_float_micropython: wrap low-level floating-point ops, to call the pico-sdk versions.
pico_wrap_function(pico_float_micropython __aeabi_fdiv)
pico_wrap_function(pico_float_micropython __aeabi_fmul)
pico_wrap_function(pico_float_micropython __aeabi_frsub)
pico_wrap_function(pico_float_micropython __aeabi_fsub)
pico_wrap_function(pico_float_micropython __aeabi_cfcmpeq)
pico_wrap_function(pico_float_micropython __aeabi_cfrcmple)
pico_wrap_function(pico_float_micropython __aeabi_cfcmple)
pico_wrap_function(pico_float_micropython __aeabi_fcmpeq)
pico_wrap_function(pico_float_micropython __aeabi_fcmplt)
pico_wrap_function(pico_float_micropython __aeabi_fcmple)
pico_wrap_function(pico_float_micropython __aeabi_fcmpge)
pico_wrap_function(pico_float_micropython __aeabi_fcmpgt)
pico_wrap_function(pico_float_micropython __aeabi_fcmpun)
pico_wrap_function(pico_float_micropython __aeabi_i2f)
pico_wrap_function(pico_float_micropython __aeabi_l2f)
pico_wrap_function(pico_float_micropython __aeabi_ui2f)
pico_wrap_function(pico_float_micropython __aeabi_ul2f)
pico_wrap_function(pico_float_micropython __aeabi_f2iz)
pico_wrap_function(pico_float_micropython __aeabi_f2lz)
pico_wrap_function(pico_float_micropython __aeabi_f2uiz)
if(PICO_RP2040)
    pico_wrap_function(pico_float_micropython __aeabi_f2ulz)
endif()
pico_wrap_function(pico_float_micropython __aeabi_f2d)

if (MICROPY_PY_LWIP)
    target_link_libraries(${MICROPY_TARGET} micropy_lib_lwip)

    target_include_directories(${MICROPY_TARGET} PRIVATE
        lwip_inc
    )
    target_compile_definitions(${MICROPY_TARGET} PRIVATE
        MICROPY_PY_LWIP=1
    )
endif()

if(MICROPY_PY_BLUETOOTH)
    list(APPEND MICROPY_SOURCE_PORT mpbthciport.c)
    target_compile_definitions(${MICROPY_TARGET} PRIVATE
        MICROPY_PY_BLUETOOTH=1
        MICROPY_PY_BLUETOOTH_USE_SYNC_EVENTS=1
        MICROPY_PY_BLUETOOTH_ENABLE_CENTRAL_MODE=1
    )
endif()

if (MICROPY_PY_BLUETOOTH_CYW43)
    target_compile_definitions(${MICROPY_TARGET} PRIVATE
        CYW43_ENABLE_BLUETOOTH=1
        MICROPY_PY_BLUETOOTH_CYW43=1
    )

    if (MICROPY_BLUETOOTH_BTSTACK)
        target_link_libraries(${MICROPY_TARGET}
            pico_btstack_hci_transport_cyw43
        )
    endif()
endif()

if (MICROPY_BLUETOOTH_BTSTACK)
    string(CONCAT GIT_SUBMODULES "${GIT_SUBMODULES} " lib/btstack)

    list(APPEND MICROPY_SOURCE_PORT mpbtstackport.c)

    include(${MICROPY_DIR}/extmod/btstack/btstack.cmake)
    target_link_libraries(${MICROPY_TARGET} micropy_extmod_btstack)

    target_compile_definitions(${MICROPY_TARGET} PRIVATE
        MICROPY_BLUETOOTH_BTSTACK=1
        MICROPY_BLUETOOTH_BTSTACK_CONFIG_FILE=\"btstack_inc/btstack_config.h\"
    )

    # For modbluetooth_btstack.c includes
    get_target_property(BTSTACK_INCLUDE micropy_extmod_btstack INTERFACE_INCLUDE_DIRECTORIES)
    list(APPEND MICROPY_INC_CORE ${BTSTACK_INCLUDE})
endif()

if(MICROPY_BLUETOOTH_NIMBLE)
    string(CONCAT GIT_SUBMODULES "${GIT_SUBMODULES} " lib/mynewt-nimble)
    if(NOT (${ECHO_SUBMODULES}) AND NOT EXISTS ${MICROPY_DIR}/lib/mynewt-nimble/nimble/host/include/host/ble_hs.h)
        message(FATAL_ERROR " mynewt-nimble not initialized.\n Run 'make BOARD=${MICROPY_BOARD} submodules'")
    endif()

    list(APPEND MICROPY_SOURCE_PORT mpnimbleport.c)
    target_compile_definitions(${MICROPY_TARGET} PRIVATE
        MICROPY_BLUETOOTH_NIMBLE=1
        MICROPY_BLUETOOTH_NIMBLE_BINDINGS_ONLY=0
        MICROPY_PY_BLUETOOTH_ENABLE_PAIRING_BONDING=1
        MICROPY_PY_BLUETOOTH_ENABLE_L2CAP_CHANNELS=1
    )
    target_compile_options(${MICROPY_TARGET} PRIVATE
    # TODO: This flag is currently needed to make nimble build.
    -Wno-unused-but-set-variable
    )
    include(${MICROPY_DIR}/extmod/nimble/nimble.cmake)
    target_link_libraries(${MICROPY_TARGET} micropy_extmod_nimble)
    get_target_property(NIMBLE_INCLUDE micropy_extmod_nimble INTERFACE_INCLUDE_DIRECTORIES)
    list(APPEND MICROPY_INC_CORE ${NIMBLE_INCLUDE})
endif()

# tinyusb helper
target_include_directories(${MICROPY_TARGET} PRIVATE
    ${MICROPY_DIR}/shared/tinyusb/
)

if (MICROPY_PY_NETWORK_CYW43)
    string(CONCAT GIT_SUBMODULES "${GIT_SUBMODULES} " lib/cyw43-driver)
    if((NOT (${ECHO_SUBMODULES})) AND NOT EXISTS ${MICROPY_DIR}/lib/cyw43-driver/src/cyw43.h)
        message(FATAL_ERROR " cyw43-driver not initialized.\n Run 'make BOARD=${MICROPY_BOARD} submodules'")
    endif()

    target_compile_definitions(${MICROPY_TARGET} PRIVATE
        MICROPY_PY_NETWORK_CYW43=1
        MICROPY_PY_SOCKET_DEFAULT_TIMEOUT_MS=30000 # default socket timeout
    )
    if (CMAKE_BUILD_TYPE MATCHES Debug)
    target_compile_definitions(${MICROPY_TARGET} PRIVATE
        CYW43_USE_STATS=1
    )
    endif()

    list(APPEND MICROPY_SOURCE_PORT
        machine_pin_cyw43.c
    )

    target_link_libraries(${MICROPY_TARGET}
        cyw43_driver_picow
    )
    target_include_directories(${MICROPY_TARGET} PRIVATE
        ${MICROPY_DIR}/lib/cyw43-driver/
    )

endif()

if (MICROPY_PY_NETWORK_NINAW10)
    target_compile_definitions(${MICROPY_TARGET} PRIVATE
        MICROPY_PY_NETWORK_NINAW10=1
    )

    target_include_directories(${MICROPY_TARGET} PRIVATE
        ${MICROPY_DIR}/drivers/ninaw10/
    )

    # Enable NINA-W10 WiFi and Bluetooth drivers.
    list(APPEND MICROPY_SOURCE_DRIVERS
        ${MICROPY_DIR}/drivers/ninaw10/nina_bt_hci.c
        ${MICROPY_DIR}/drivers/ninaw10/nina_wifi_drv.c
        ${MICROPY_DIR}/drivers/ninaw10/nina_wifi_bsp.c
        ${MICROPY_DIR}/drivers/ninaw10/machine_pin_nina.c
    )
endif()

if (MICROPY_PY_NETWORK_WIZNET5K)
    string(CONCAT GIT_SUBMODULES "${GIT_SUBMODULES} " lib/wiznet5k)
    if((NOT (${ECHO_SUBMODULES})) AND NOT EXISTS ${MICROPY_DIR}/lib/wiznet5k/README.md)
        message(FATAL_ERROR " wiznet5k not initialized.\n Run 'make BOARD=${MICROPY_BOARD} submodules'")
    endif()

    target_compile_definitions(${MICROPY_TARGET} PRIVATE
        MICROPY_PY_NETWORK_WIZNET5K=1
        WIZCHIP_PREFIXED_EXPORTS=1
        _WIZCHIP_=${MICROPY_PY_NETWORK_WIZNET5K}
        WIZCHIP_YIELD=mpy_wiznet_yield
    )

    if (MICROPY_PY_LWIP)
        target_compile_definitions(${MICROPY_TARGET} PRIVATE
            # When using MACRAW mode (with lwIP), maximum buffer space must be used for the raw socket
            WIZCHIP_USE_MAX_BUFFER=1
        )
    endif()

    target_include_directories(${MICROPY_TARGET} PRIVATE
        ${MICROPY_DIR}/lib/wiznet5k/
        ${MICROPY_DIR}/lib/wiznet5k/Ethernet/
    )

    list(APPEND MICROPY_SOURCE_LIB
        ${MICROPY_DIR}/lib/wiznet5k/Ethernet/W5100/w5100.c
        ${MICROPY_DIR}/lib/wiznet5k/Ethernet/W5100S/w5100s.c
        ${MICROPY_DIR}/lib/wiznet5k/Ethernet/W5200/w5200.c
        ${MICROPY_DIR}/lib/wiznet5k/Ethernet/W5300/w5300.c
        ${MICROPY_DIR}/lib/wiznet5k/Ethernet/W5500/w5500.c
        ${MICROPY_DIR}/lib/wiznet5k/Ethernet/socket.c
        ${MICROPY_DIR}/lib/wiznet5k/Ethernet/wizchip_conf.c
        ${MICROPY_DIR}/lib/wiznet5k/Internet/DNS/dns.c
        ${MICROPY_DIR}/lib/wiznet5k/Internet/DHCP/dhcp.c
    )
endif()

# Add qstr sources for extmod and usermod, in case they are modified by components above.
list(APPEND MICROPY_SOURCE_QSTR
    ${MICROPY_SOURCE_EXTMOD}
    ${MICROPY_SOURCE_USERMOD}
    ${MICROPY_SOURCE_BOARD}
)

# Define mpy-cross flags
set(MICROPY_CROSS_FLAGS -march=armv6m)

# Set the frozen manifest file
if (MICROPY_USER_FROZEN_MANIFEST)
    set(MICROPY_FROZEN_MANIFEST ${MICROPY_USER_FROZEN_MANIFEST})
elseif (NOT MICROPY_FROZEN_MANIFEST)
    set(MICROPY_FROZEN_MANIFEST ${MICROPY_PORT_DIR}/boards/manifest.py)
endif()

target_sources(${MICROPY_TARGET} PRIVATE
    ${MICROPY_SOURCE_PY}
    ${MICROPY_SOURCE_EXTMOD}
    ${MICROPY_SOURCE_LIB}
    ${MICROPY_SOURCE_DRIVERS}
    ${MICROPY_SOURCE_PORT}
    ${MICROPY_SOURCE_BOARD}
)

target_link_libraries(${MICROPY_TARGET} micropy_lib_mbedtls)

target_link_libraries(${MICROPY_TARGET} usermod)

target_include_directories(${MICROPY_TARGET} PRIVATE
    ${MICROPY_INC_CORE}
    ${MICROPY_INC_USERMOD}
    ${MICROPY_BOARD_DIR}
    "${MICROPY_PORT_DIR}"
    "${CMAKE_BINARY_DIR}"
)

target_compile_options(${MICROPY_TARGET} PRIVATE
    -Wall
    -Werror
    -g  # always include debug information in the ELF
)

target_link_options(${MICROPY_TARGET} PRIVATE
    -Wl,--defsym=__micropy_c_heap_size__=${MICROPY_C_HEAP_SIZE}
    -Wl,--wrap=dcd_event_handler
    -Wl,--wrap=runtime_init_clocks
)

if(PICO_RP2350)
    target_link_options(${MICROPY_TARGET} PRIVATE
        -Wl,--defsym=__micropy_extra_stack__=4096
    )
endif()

# Apply optimisations to performance-critical source code.
set_source_files_properties(
    ${MICROPY_PY_DIR}/map.c
    ${MICROPY_PY_DIR}/mpz.c
    ${MICROPY_PY_DIR}/vm.c
    PROPERTIES
    COMPILE_OPTIONS "-O2"
)

set_source_files_properties(
    ${PICO_SDK_PATH}/src/rp2_common/pico_double/double_math.c
    ${PICO_SDK_PATH}/src/rp2_common/pico_float/float_math.c
    PROPERTIES
    COMPILE_OPTIONS "-Wno-error=uninitialized"
)

set_source_files_properties(
    ${PICO_TINYUSB_PATH}/src/portable/raspberrypi/rp2040/dcd_rp2040.c
    ${PICO_TINYUSB_PATH}/src/portable/raspberrypi/rp2040/rp2040_usb.c
    PROPERTIES
    COMPILE_OPTIONS "-Wno-error=array-bounds;-Wno-error=unused-but-set-variable"
)

target_compile_definitions(${MICROPY_TARGET} PRIVATE
    ${MICROPY_DEF_BOARD}
    FFCONF_H=\"${MICROPY_OOFATFS_DIR}/ffconf.h\"
    LFS1_NO_MALLOC LFS1_NO_DEBUG LFS1_NO_WARN LFS1_NO_ERROR LFS1_NO_ASSERT
    LFS2_NO_MALLOC LFS2_NO_DEBUG LFS2_NO_WARN LFS2_NO_ERROR LFS2_NO_ASSERT
    PICO_FLOAT_PROPAGATE_NANS=1
    PICO_STACK_SIZE=0x2000
    PICO_CORE1_STACK_SIZE=0
    PICO_MAX_SHARED_IRQ_HANDLERS=8 # we need more than the default
    PICO_PROGRAM_NAME="MicroPython"
    PICO_NO_PROGRAM_VERSION_STRING=1 # do it ourselves in main.c
    MICROPY_BUILD_TYPE="${CMAKE_C_COMPILER_ID} ${CMAKE_C_COMPILER_VERSION} ${CMAKE_BUILD_TYPE}"
    PICO_NO_BI_STDIO_UART=1 # we call it UART REPL
)

if(PICO_RP2040)
    target_compile_definitions(${MICROPY_TARGET} PRIVATE
        PICO_RP2040_USB_DEVICE_ENUMERATION_FIX=1
    )
elseif(PICO_RP2350)
    target_compile_definitions(${MICROPY_TARGET} PRIVATE
        PICO_EMBED_XIP_SETUP=1 # to put flash into continuous read mode
    )
endif()

target_link_libraries(${MICROPY_TARGET}
    ${PICO_SDK_COMPONENTS}
)

if (MICROPY_HW_ENABLE_DOUBLE_TAP)
# Enable double tap reset into bootrom.
target_link_libraries(${MICROPY_TARGET}
    pico_bootsel_via_double_reset
)
endif()

# todo this is a bit brittle, but we want to move a few source files into RAM (which requires
#  a linker script modification) until we explicitly add  macro calls around the function
#  defs to move them into RAM.
if (PICO_ON_DEVICE AND NOT PICO_NO_FLASH AND NOT PICO_COPY_TO_RAM)
    if(PICO_RP2040)
        pico_set_linker_script(${MICROPY_TARGET} ${CMAKE_CURRENT_LIST_DIR}/memmap_mp_rp2040.ld)
    elseif(PICO_RP2350)
        pico_set_linker_script(${MICROPY_TARGET} ${CMAKE_CURRENT_LIST_DIR}/memmap_mp_rp2350.ld)
    endif()
endif()

pico_add_extra_outputs(${MICROPY_TARGET})

pico_find_compiler_with_triples(PICO_COMPILER_SIZE "${PICO_GCC_TRIPLE}" size)

add_custom_command(TARGET ${MICROPY_TARGET}
    POST_BUILD
    COMMAND ${PICO_COMPILER_SIZE} --format=berkeley ${PROJECT_BINARY_DIR}/${MICROPY_TARGET}.elf
    VERBATIM
)

# Collect all the include directories and compile definitions for the pico-sdk components.
foreach(comp ${PICO_SDK_COMPONENTS})
    micropy_gather_target_properties(${comp})
    micropy_gather_target_properties(${comp}_headers)
endforeach()

set(MICROPY_CPP_FLAGS_EXTRA ${PICO_COMMON_LANG_FLAGS})
separate_arguments(MICROPY_CPP_FLAGS_EXTRA)

# Include the main MicroPython cmake rules.
include(${MICROPY_DIR}/py/mkrules.cmake)

set(GEN_PINS_AF_CSV "${MICROPY_PORT_DIR}/boards/rp2_af.csv")
set(GEN_PINS_PREFIX "${MICROPY_PORT_DIR}/boards/rp2_prefix.c")
set(GEN_PINS_MKPINS "${MICROPY_PORT_DIR}/boards/make-pins.py")
set(GEN_PINS_SRC "${CMAKE_BINARY_DIR}/pins_${MICROPY_BOARD}.c")
set(GEN_PINS_HDR "${MICROPY_GENHDR_DIR}/pins.h")

if(NOT PICO_NUM_GPIOS)
    set(PICO_NUM_GPIOS 30)
endif()

if(NOT PICO_NUM_EXT_GPIOS)
    set(PICO_NUM_EXT_GPIOS 10)
endif()

if(EXISTS "${MICROPY_BOARD_DIR}/pins.csv")
    set(GEN_PINS_BOARD_CSV "${MICROPY_BOARD_DIR}/pins.csv")
    set(GEN_PINS_CSV_ARG --board-csv "${GEN_PINS_BOARD_CSV}")
endif()

target_sources(${MICROPY_TARGET} PRIVATE
    ${GEN_PINS_HDR}
)

# Generate pins
add_custom_command(
    OUTPUT ${GEN_PINS_HDR} ${GEN_PINS_SRC}
    COMMAND ${Python3_EXECUTABLE} ${GEN_PINS_MKPINS} ${GEN_PINS_CSV_ARG} --af-csv ${GEN_PINS_AF_CSV} --prefix ${GEN_PINS_PREFIX} --output-source ${GEN_PINS_SRC} --num-gpios ${PICO_NUM_GPIOS} --num-ext-gpios ${PICO_NUM_EXT_GPIOS} --output-header ${GEN_PINS_HDR}
    DEPENDS
        ${GEN_PINS_AF_CSV}
        ${GEN_PINS_BOARD_CSV}
        ${GEN_PINS_MKPINS}
        ${GEN_PINS_PREFIX}
        ${MICROPY_MPVERSION}
    VERBATIM
    COMMAND_EXPAND_LISTS
)

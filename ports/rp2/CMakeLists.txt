cmake_minimum_required(VERSION 3.12)

# Set build type to reduce firmware size
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE MinSizeRel)
endif()

# Set main target and component locations
set(MICROPYTHON_TARGET firmware)
get_filename_component(MPY_DIR "../.." ABSOLUTE)
if (PICO_SDK_PATH_OVERRIDE)
    set(PICO_SDK_PATH ${PICO_SDK_PATH_OVERRIDE})
else()
    set(PICO_SDK_PATH ../../lib/pico-sdk)
endif()

# Use the local tinyusb instead of the one in pico-sdk
set(PICO_TINYUSB_PATH ${MPY_DIR}/lib/tinyusb)

# Include component cmake fragments
include(micropy_py.cmake)
include(micropy_extmod.cmake)
include(${PICO_SDK_PATH}/pico_sdk_init.cmake)

# Define the top-level project
project(${MICROPYTHON_TARGET})

pico_sdk_init()

add_executable(${MICROPYTHON_TARGET})

set(SOURCE_LIB
    ${MPY_DIR}/lib/littlefs/lfs1.c
    ${MPY_DIR}/lib/littlefs/lfs1_util.c
    ${MPY_DIR}/lib/littlefs/lfs2.c
    ${MPY_DIR}/lib/littlefs/lfs2_util.c
    ${MPY_DIR}/lib/mp-readline/readline.c
    ${MPY_DIR}/lib/oofatfs/ff.c
    ${MPY_DIR}/lib/oofatfs/ffunicode.c
    ${MPY_DIR}/lib/timeutils/timeutils.c
    ${MPY_DIR}/lib/utils/gchelper_m0.s
    ${MPY_DIR}/lib/utils/gchelper_native.c
    ${MPY_DIR}/lib/utils/mpirq.c
    ${MPY_DIR}/lib/utils/stdout_helpers.c
    ${MPY_DIR}/lib/utils/sys_stdio_mphal.c
    ${MPY_DIR}/lib/utils/pyexec.c
)

set(SOURCE_DRIVERS
    ${MPY_DIR}/drivers/bus/softspi.c
)

set(SOURCE_PORT
    machine_adc.c
    machine_i2c.c
    machine_pin.c
    machine_pwm.c
    machine_spi.c
    machine_timer.c
    machine_uart.c
    machine_wdt.c
    machine_rtc.c
    main.c
    modmachine.c
    modrp2.c
    moduos.c
    modutime.c
    mphalport.c
    mpthreadport.c
    rp2_flash.c
    rp2_pio.c
    tusb_port.c
    uart.c
)

set(SOURCE_QSTR
    ${SOURCE_PY}
    ${SOURCE_EXTMOD}
    ${MPY_DIR}/lib/utils/mpirq.c
    ${MPY_DIR}/lib/utils/sys_stdio_mphal.c
    ${PROJECT_SOURCE_DIR}/machine_adc.c
    ${PROJECT_SOURCE_DIR}/machine_i2c.c
    ${PROJECT_SOURCE_DIR}/machine_pin.c
    ${PROJECT_SOURCE_DIR}/machine_pwm.c
    ${PROJECT_SOURCE_DIR}/machine_spi.c
    ${PROJECT_SOURCE_DIR}/machine_timer.c
    ${PROJECT_SOURCE_DIR}/machine_uart.c
    ${PROJECT_SOURCE_DIR}/machine_wdt.c
    ${PROJECT_SOURCE_DIR}/machine_rtc.c
    ${PROJECT_SOURCE_DIR}/modmachine.c
    ${PROJECT_SOURCE_DIR}/modrp2.c
    ${PROJECT_SOURCE_DIR}/moduos.c
    ${PROJECT_SOURCE_DIR}/modutime.c
    ${PROJECT_SOURCE_DIR}/rp2_flash.c
    ${PROJECT_SOURCE_DIR}/rp2_pio.c
)

set(MPY_QSTR_DEFS ${PROJECT_SOURCE_DIR}/qstrdefsport.h)

# Define mpy-cross flags and frozen manifest
set(MPY_CROSS_FLAGS -march=armv7m)
set(FROZEN_MANIFEST ${PROJECT_SOURCE_DIR}/manifest.py)

include(micropy_rules.cmake)

target_sources(${MICROPYTHON_TARGET} PRIVATE
    ${SOURCE_PY}
    ${SOURCE_EXTMOD}
    ${SOURCE_LIB}
    ${SOURCE_DRIVERS}
    ${SOURCE_PORT}
)

target_include_directories(${MICROPYTHON_TARGET} PRIVATE
                           "${PROJECT_SOURCE_DIR}"
                           "${MPY_DIR}"
                           "${CMAKE_BINARY_DIR}"
                           )

target_compile_options(${MICROPYTHON_TARGET} PRIVATE
    -Wall
    #-Werror
    -DFFCONF_H=\"${MPY_DIR}/lib/oofatfs/ffconf.h\"
    -DLFS1_NO_MALLOC -DLFS1_NO_DEBUG -DLFS1_NO_WARN -DLFS1_NO_ERROR -DLFS1_NO_ASSERT
    -DLFS2_NO_MALLOC -DLFS2_NO_DEBUG -DLFS2_NO_WARN -DLFS2_NO_ERROR -DLFS2_NO_ASSERT
)

target_compile_definitions(${MICROPYTHON_TARGET} PRIVATE
    PICO_FLOAT_PROPAGATE_NANS=1
    PICO_STACK_SIZE=0x2000
    PICO_CORE1_STACK_SIZE=0
    PICO_PROGRAM_NAME="MicroPython"
    PICO_NO_PROGRAM_VERSION_STRING=1 # do it ourselves in main.c
    MICROPY_BUILD_TYPE="${CMAKE_C_COMPILER_ID} ${CMAKE_C_COMPILER_VERSION} ${CMAKE_BUILD_TYPE}"
    PICO_NO_BI_STDIO_UART=1 # we call it UART REPL
)

target_link_libraries(${MICROPYTHON_TARGET}
    hardware_adc
    hardware_dma
    hardware_flash
    hardware_i2c
    hardware_pio
    hardware_pwm
    hardware_rtc
    hardware_spi
    hardware_sync
    pico_multicore
    pico_stdlib_headers
    pico_stdlib
    pico_unique_id
    tinyusb_device
)

# todo this is a bit brittle, but we want to move a few source files into RAM (which requires
#  a linker script modification) until we explicitly add  macro calls around the function
#  defs to move them into RAM.
if (PICO_ON_DEVICE AND NOT PICO_NO_FLASH AND NOT PICO_COPY_TO_RAM)
    pico_set_linker_script(${MICROPYTHON_TARGET} ${CMAKE_CURRENT_LIST_DIR}/memmap_mp.ld)
endif()

pico_add_extra_outputs(${MICROPYTHON_TARGET})

add_custom_command(TARGET ${MICROPYTHON_TARGET}
    POST_BUILD
    COMMAND arm-none-eabi-size --format=berkeley ${PROJECT_BINARY_DIR}/${MICROPYTHON_TARGET}.elf
    VERBATIM
)

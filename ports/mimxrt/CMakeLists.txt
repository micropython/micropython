cmake_minimum_required(VERSION 3.12)

set( CMAKE_VERBOSE_MAKEFILE on )
enable_language(C ASM)
set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)
set(toolchain_file "./arm-none-eabi-gcc.cmake")

# Set main target and component locations
set(MICROPY_TARGET firmware)
get_filename_component(MICROPY_DIR "../.." ABSOLUTE)
if (MIMXRT_SDK_PATH_OVERRIDE)
    set(MIMXRT_SDK_PATH ${MIMXRT_SDK_PATH_OVERRIDE})
else()
    set(MIMXRT_SDK_PATH ../../lib/nxp_driver/sdk/devices)
endif()

if (MIMXRT_CMSIS_PATH_OVERRIDE)
    set(MIMXRT_CMSIS_PATH ${MIMXRT_CMSIS_PATH_OVERRIDE})
else()
    set(MIMXRT_CMSIS_PATH ../../lib/nxp_driver/sdk/CMSIS)
endif()

# Set the location of this port's directory.
set(MICROPY_PORT_DIR ${CMAKE_SOURCE_DIR})

# Set the board if it's not already set.
if(NOT MICROPY_BOARD)
    set(MICROPY_BOARD TEENSY40)
endif()

# Set the board directory and check that it exists.
if(NOT MICROPY_BOARD_DIR)
    set(MICROPY_BOARD_DIR ${MICROPY_PORT_DIR}/boards/${MICROPY_BOARD})
endif()
if(NOT EXISTS ${MICROPY_BOARD_DIR}/mpconfigboard.cmake)
    message(FATAL_ERROR "Invalid MICROPY_BOARD specified: ${MICROPY_BOARD}")
endif()

# Include board config
message(${MICROPY_BOARD_DIR}/mpconfigboard.cmake)
include(${MICROPY_BOARD_DIR}/mpconfigboard.cmake)

# Include component cmake fragments
include(${MICROPY_DIR}/py/py.cmake)
include(${MICROPY_DIR}/extmod/extmod.cmake)

# Define the top-level project
project(${MICROPY_TARGET})

include(${MICROPY_DIR}/py/usermod.cmake)

add_executable(${MICROPY_TARGET})

set(MICROPY_QSTRDEFS_PORT
    ${PROJECT_SOURCE_DIR}/qstrdefsport.h
)

set(MICROPY_SOURCE_LIB
    ${MICROPY_DIR}/lib/mp-readline/readline.c
    ${MICROPY_DIR}/lib/libc/string0.c
    ${MICROPY_DIR}/lib/utils/gchelper_native.c
    ${MICROPY_DIR}/lib/utils/printf.c
    ${MICROPY_DIR}/lib/utils/pyexec.c
    ${MICROPY_DIR}/lib/utils/stdout_helpers.c
)

set(MICROPY_SOURCE_DRIVERS

)

set(MICROPY_SOURCE_PORT
    led.c
    pin.c
    machine_led.c
    main.c
    modmachine.c
    board_init.c
    modutime.c
    mphalport.c
    tusb_port.c
    ${MIMXRT_SDK_PATH}/${MCU_SERIES}/system_${MCU_SERIES}.c
    ${MIMXRT_SDK_PATH}/${MCU_SERIES}/xip/fsl_flexspi_nor_boot.c
    ${MIMXRT_SDK_PATH}/${MCU_SERIES}/project_template/clock_config.c
    ${MIMXRT_SDK_PATH}/${MCU_SERIES}/drivers/fsl_clock.c
    ${MIMXRT_SDK_PATH}/${MCU_SERIES}/drivers/fsl_gpio.c
    ${MIMXRT_SDK_PATH}/${MCU_SERIES}/drivers/fsl_common.c
    ${MIMXRT_SDK_PATH}/${MCU_SERIES}/drivers/fsl_lpuart.c
    ${MIMXRT_SDK_PATH}/${MCU_SERIES}/drivers/fsl_flexram.c
)

set(MICROPY_SOURCE_QSTR
    ${PROJECT_SOURCE_DIR}/machine_led.c
    ${PROJECT_SOURCE_DIR}/modutime.c
    ${PROJECT_SOURCE_DIR}/modmachine.c
    ${PROJECT_SOURCE_DIR}/pin.c
    ${MICROPY_BOARD_DIR}/pins.c
)

set(MIMXRT_SDK_COMPONENTS

)

# Define mpy-cross flags and frozen manifest
set(MICROPY_CROSS_FLAGS -march=armv7m)
#set(MICROPY_FROZEN_MANIFEST ${PROJECT_SOURCE_DIR}/boards/manifest.py)

target_sources(${MICROPY_TARGET} PRIVATE
    ${MICROPY_SOURCE_PY}
    ${MICROPY_SOURCE_EXTMOD}
    ${MICROPY_SOURCE_LIB}
    ${MICROPY_SOURCE_DRIVERS}
    ${MICROPY_SOURCE_PORT}
)

target_include_directories(${MICROPY_TARGET} PRIVATE
    ${MICROPY_INC_CORE}
    ${MICROPY_INC_USERMOD}
    ${MICROPY_BOARD_DIR}
    ${MIMXRT_CMSIS_PATH}/Include
    ${MIMXRT_SDK_PATH}/${MCU_SERIES}
    ${MIMXRT_SDK_PATH}/${MCU_SERIES}/drivers
    "${PROJECT_SOURCE_DIR}"
    "${CMAKE_BINARY_DIR}"
)

target_compile_options(${MICROPY_TARGET} PRIVATE
    -mtune=cortex-m7
    -mcpu=cortex-m7
    -mfloat-abi=hard
    -mfpu=fpv5-d16
    -Wall
    -Werror
    -Wdouble-promotion
    -Wfloat-conversion
    -std=c99
    -nostdlib
    -mthumb
    -Wno-error=unused-parameter
    -fdata-sections
    -ffunction-sections
)

target_compile_definitions(${MICROPY_TARGET} PRIVATE
    CPU_${MCU_SERIES}
    CPU_${MCU_VARIANT}
    CPU_HEADER_H=<${MCU_SERIES}.h>
    __STARTUP_CLEAR_BSS
    __START=main
    XIP_EXTERNAL_FLASH=1
    XIP_BOOT_HEADER_ENABLE=1
    CFG_TUSB_MCU=OPT_MCU_MIMXRT10XX
)

set_target_properties(${MICROPY_TARGET}
    PROPERTIES LINK_DEPENDS ${MICROPY_PORT_DIR}/boards/${MCU_SERIES}.ld
)

set_target_properties(${MICROPY_TARGET}
    PROPERTIES LINK_DEPENDS ${MIMXRT_SDK_PATH}/${MCU_SERIES}/gcc/${MCU_SERIES}xxxxx_flexspi_nor.ld
)

# Include the main MicroPython cmake rules.
include(${MICROPY_DIR}/py/mkrules.cmake)

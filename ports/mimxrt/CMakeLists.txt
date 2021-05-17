cmake_minimum_required(VERSION 3.12)

set( CMAKE_VERBOSE_MAKEFILE OFF)
enable_language(C ASM)
set(CMAKE_EXECUTABLE_SUFFIX .elf)

# Set main target and component locations
set(MICROPY_TARGET firmware)
get_filename_component(MICROPY_DIR "../.." ABSOLUTE)
if (MIMXRT_SDK_PATH_OVERRIDE)
    set(MIMXRT_SDK_PATH ${MIMXRT_SDK_PATH_OVERRIDE})
else()
    set(MIMXRT_SDK_PATH ${MICROPY_DIR}/lib/nxp_driver/sdk/devices)
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
    ${MICROPY_DIR}/lib/utils/gchelper_m3.s
    ${MICROPY_DIR}/lib/utils/gchelper_native.c
    ${MICROPY_DIR}/lib/utils/printf.c
    ${MICROPY_DIR}/lib/utils/pyexec.c
    ${MICROPY_DIR}/lib/utils/stdout_helpers.c
)

set(MICROPY_SOURCE_DRIVERS

)

set(TINY_USB_SOURCE
    ${MICROPY_DIR}/lib/tinyusb/src/tusb.c
    ${MICROPY_DIR}/lib/tinyusb/src/common/tusb_fifo.c
    ${MICROPY_DIR}/lib/tinyusb/src/device/usbd.c
    ${MICROPY_DIR}/lib/tinyusb/src/device/usbd_control.c
    ${MICROPY_DIR}/lib/tinyusb/src/class/msc/msc_device.c
    ${MICROPY_DIR}/lib/tinyusb/src/class/cdc/cdc_device.c
    ${MICROPY_DIR}/lib/tinyusb/src/class/dfu/dfu_rt_device.c
    ${MICROPY_DIR}/lib/tinyusb/src/class/hid/hid_device.c
    ${MICROPY_DIR}/lib/tinyusb/src/class/midi/midi_device.c
    ${MICROPY_DIR}/lib/tinyusb/src/class/usbtmc/usbtmc_device.c
    ${MICROPY_DIR}/lib/tinyusb/src/class/vendor/vendor_device.c
    ${MICROPY_DIR}/lib/tinyusb/src/portable/nxp/transdimension/dcd_transdimension.c
)

set(MICROPY_SOURCE_PORT
    board_init.c
    led.c
    machine_led.c
    main.c
    modmachine.c
    modutime.c
    mphalport.c
    pin.c
    tusb_port.c
    ${MICROPY_BOARD_DIR}/flash_config.c
    ${MICROPY_BOARD_DIR}/pins.c
    ${MIMXRT_SDK_PATH}/${MCU_SERIES}/gcc/startup_${MCU_SERIES}.S
    ${MIMXRT_SDK_PATH}/${MCU_SERIES}/system_${MCU_SERIES}.c
    ${MIMXRT_SDK_PATH}/${MCU_SERIES}/xip/fsl_flexspi_nor_boot.c
    ${MIMXRT_SDK_PATH}/${MCU_SERIES}/project_template/clock_config.c
    ${MIMXRT_SDK_PATH}/${MCU_SERIES}/drivers/fsl_clock.c
    ${MIMXRT_SDK_PATH}/${MCU_SERIES}/drivers/fsl_gpio.c
    ${MIMXRT_SDK_PATH}/${MCU_SERIES}/drivers/fsl_common.c
    ${MIMXRT_SDK_PATH}/${MCU_SERIES}/drivers/fsl_lpuart.c
    ${MIMXRT_SDK_PATH}/${MCU_SERIES}/drivers/fsl_flexram.c
    ${TINY_USB_SOURCE}
)

set(MICROPY_SOURCE_QSTR
    ${MICROPY_SOURCE_PY}
    ${MICROPY_SOURCE_EXTMOD}
    ${MICROPY_SOURCE_LIB}
    ${MICROPY_SOURCE_USERMOD}
    ${MICROPY_PORT_DIR}/board_init.c
    ${MICROPY_PORT_DIR}/led.c
    ${MICROPY_PORT_DIR}/machine_led.c
    ${MICROPY_PORT_DIR}/main.c
    ${MICROPY_PORT_DIR}/modmachine.c
    ${MICROPY_PORT_DIR}/modutime.c
    ${MICROPY_PORT_DIR}/mphalport.c
    ${MICROPY_PORT_DIR}/pin.c
    ${MICROPY_PORT_DIR}/tusb_port.c
    ${MICROPY_BOARD_DIR}/flash_config.c
    ${MICROPY_BOARD_DIR}/pins.c
)

# Define mpy-cross flags and frozen manifest
set(MICROPY_CROSS_FLAGS -march=armv7-m)
#set(MICROPY_FROZEN_MANIFEST ${PROJECT_SOURCE_DIR}/boards/manifest.py)

target_link_libraries(${MICROPY_TARGET} micropy_extmod_btree)
target_compile_options(micropy_extmod_btree PRIVATE
    -march=armv7-m
    -mcpu=cortex-m7
    -mfloat-abi=hard
    -mfpu=fpv5-d16
    -mthumb
    -Os
    -DNDEBUG
)

target_link_libraries(${MICROPY_TARGET} usermod)
target_link_libraries(${MICROPY_TARGET} -lgcc)

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
    ${MICROPY_DIR}/lib/cmsis/inc
    ${MIMXRT_SDK_PATH}/${MCU_SERIES}
    ${MIMXRT_SDK_PATH}/${MCU_SERIES}/drivers
    ${MIMXRT_SDK_PATH}/${MCU_SERIES}/project_template
    ${MICROPY_DIR}/lib/tinyusb/src
    ${MICROPY_DIR}/lib/tinyusb/hw
    ${MICROPY_DIR}/lib/tinyusb/hw/bsp/teensy_40
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

target_link_options(${MICROPY_TARGET} PRIVATE
    -T${MICROPY_PORT_DIR}/boards/${MCU_SERIES}.ld
    -T${MIMXRT_SDK_PATH}/${MCU_SERIES}/gcc/${MCU_SERIES}xxxxx_flexspi_nor.ld
    -mtune=cortex-m7
    -mcpu=cortex-m7
    -mfloat-abi=hard
    -mfpu=fpv5-d16
    -Wall
    -Werror
    -Wdouble-promotion
    -Wfloat-conversion
    -Wl,--print-memory-usage
    -std=c99
    -nostdlib
    -mthumb
    -lm
    -lc
    -lnosys
    -Wno-error=unused-parameter
    -fdata-sections
    -ffunction-sections
    -Wl,-Map=${PROJECT_NAME}.map,--cref
    -Wl,--gc-sections
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

add_custom_command(TARGET ${MICROPY_TARGET}
    POST_BUILD
    COMMAND arm-none-eabi-size --format=berkeley ${PROJECT_BINARY_DIR}/${MICROPY_TARGET}.elf
    VERBATIM
)

# Include the main MicroPython cmake rules.
include(${MICROPY_DIR}/py/mkrules.cmake)

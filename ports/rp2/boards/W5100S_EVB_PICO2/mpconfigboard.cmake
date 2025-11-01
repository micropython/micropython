# cmake file for Wiznet W5100S-EVB-Pico2.
set(PICO_BOARD "pico2")
set(MICROPY_PY_NETWORK_WIZNET6K W5100S)
set(MICROPY_PY_LWIP 1)
set(MICROPY_WIZNET_PIO 0)
set(MICROPY_FROZEN_MANIFEST ${MICROPY_BOARD_DIR}/manifest.py)
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wno-unused-but-set-variable -Wno-unused-variable -Wno-misleading-indentation -Wno-incompatible-pointer-types -Wno-error=unused-function -Wno-comment")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -mthumb -mcpu=cortex-m33")

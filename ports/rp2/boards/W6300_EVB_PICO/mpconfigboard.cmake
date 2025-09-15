# cmake file for Wiznet W6300-EVB-Pico.
set(PICO_BOARD wiznet_w5100s_evb_pico)
set(MICROPY_PY_NETWORK_WIZNET6K W6300)
set(MICROPY_PY_LWIP 1)
set(MICROPY_PY_NETWORK 1)
set(MICROPY_WIZNET_PIO 1)
set(MICROPY_FROZEN_MANIFEST ${MICROPY_BOARD_DIR}/manifest.py)
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wno-unused-but-set-variable -Wno-unused-variable -Wno-misleading-indentation -Wno-incompatible-pointer-types -Wno-error=unused-function -Wno-comment")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -mthumb -mcpu=cortex-m0plus")

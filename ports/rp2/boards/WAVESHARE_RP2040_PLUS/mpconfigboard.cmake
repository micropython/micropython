# CMake file for Waveshare RP2040-Plus boards

# Provide different variants for the downloads page
set(BOARD_VARIANTS "flash_4mb flash_16mb")

# Select the 4MB variant as the default
set(PICO_BOARD "waveshare_rp2040_plus_4mb")

if("${BOARD_VARIANT}" STREQUAL "flash_16mb")
set(PICO_BOARD "waveshare_rp2040_plus_16mb")
endif()

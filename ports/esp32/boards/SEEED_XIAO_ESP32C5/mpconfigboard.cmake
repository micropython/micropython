include(boards/mpconfigboard_esp32c5_common.cmake)

# XIAO ESP32C5 has 8MB flash and 8MB quad SPIRAM (enabled by the common config).
list(APPEND SDKCONFIG_DEFAULTS
    boards/sdkconfig.flash_qio_80m)

set(IDF_TARGET esp32c2)

set(SDKCONFIG_DEFAULTS
    boards/sdkconfig.base
    boards/sdkconfig.ble
    boards/sdkconfig.c2
    # C2 has unusably low free RAM without these optimisations
    boards/sdkconfig.free_ram
)

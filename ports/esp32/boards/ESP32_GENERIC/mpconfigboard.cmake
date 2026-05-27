set(SDKCONFIG_DEFAULTS
    boards/sdkconfig.base
    boards/sdkconfig.ble
)

if(MICROPY_PY_ESP32_LWIP_NAT)
    list(APPEND SDKCONFIG_DEFAULTS
        boards/sdkconfig.nat
    )
endif()

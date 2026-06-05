if(CONFIG_DRIVER_ILI9341_ENABLE)
    set(srcs
        "driver_ili9341.c"
        "esp32-component-spi-ili9341/ili9341.c"
    )
    set(includes
    "include"
    "esp32-component-spi-ili9341/include"
    )
else()
    set(srcs "")
    set(includes
    ""
    )
endif()

idf_component_register(SRCS "${srcs}"
                       INCLUDE_DIRS ${includes}
                       REQUIRES micropython driver_framebuffer)

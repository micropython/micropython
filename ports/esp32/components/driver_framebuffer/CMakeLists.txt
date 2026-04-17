if(CONFIG_DRIVER_FRAMEBUFFER_ENABLE)
set(srcs
    "driver_framebuffer.cpp"
    "driver_framebuffer_dirty.cpp"
    "driver_framebuffer_drawing.cpp"
    "driver_framebuffer_orientation.cpp"
    "driver_framebuffer_text.cpp"
    "driver_framebuffer_window.cpp"
    "driver_framebuffer_matrix.cpp"
    "driver_framebuffer_matrix_drawing.cpp"
    "moddisplay.c"
    "png/adler32.c"
    "png/crc32.c"
    "png/deflate_reader.c"
    "png/file_reader.c"
    "png/flash_reader.c"
    "png/mem_reader.c"
    "png/png_reader.c"
    "fonts/font_org_01_8pt7b.c"
	"fonts/font_org_01_8pt7b.c"
	"fonts/font_fairlight_8pt7b.c"
	"fonts/font_fairlight_12pt7b.c"
	"fonts/font_dejavusans_20pt7b.c"
	"fonts/font_permanentmarker_22pt7b.c"
	"fonts/font_permanentmarker_36pt7b.c"
	"fonts/font_roboto-Black_22pt7b.c"
	"fonts/font_roboto-BlackItalic_24pt7b.c"
	"fonts/font_roboto_12pt7b.c"
	"fonts/font_roboto_18pt7b.c"
	"fonts/font_roboto_22pt7b.c"
	"fonts/font_weather42pt8b.c"
	"fonts/font_roboto_12pt7b.c"
	"fonts/font_ipane7x5.c"
	"fonts/font_ocra_16pt7b.c"
	"fonts/font_ocra_22pt7b.c"
	"fonts/font_Exo2-Regular_22pt7b.c"
	"fonts/font_Exo2-Thin_22pt7b.c"
	"fonts/font_Exo2-Bold_22pt7b.c"
	"fonts/font_Exo2-Regular_18pt7b.c"
	"fonts/font_Exo2-Thin_18pt7b.c"
	"fonts/font_Exo2-Bold_18pt7b.c"
	"fonts/font_Exo2-Regular_12pt7b.c"
	"fonts/font_Exo2-Thin_12pt7b.c"
	"fonts/font_Exo2-Bold_12pt7b.c"
        "fonts/font_PressStart2P_6pt7b.c"
        "fonts/font_PressStart2P_8pt7b.c"
        "fonts/font_PressStart2P_9pt7b.c"
        "fonts/font_PressStart2P_12pt7b.c"
        "fonts/font_PressStart2P_18pt7b.c"
        "fonts/font_PressStart2P_22pt7b.c"
)
else()
set(srcs
    "png/adler32.c"
    "png/crc32.c"
    "png/deflate_reader.c"
    "png/file_reader.c"
    "png/flash_reader.c"
    "png/mem_reader.c"
    "png/png_reader.c")
endif()

set(IDF_COMPONENTS
    micropython
    buses
    driver_display_ili9341
    driver_badge_bsp
#    driver_display_depg0290b1
#    driver_display_ssd1306
#    driver_io_disobey_samd
    spi_flash
	esp_partition
	esp_timer
	esp_mm
    )

set(includes
    "include"
    "png"
)

# Compute the MicroPython root relative to this component
set(MP_ROOT ${PROJECT_DIR}/../..)

idf_component_register(
	SRCS "${srcs}"
    PRIV_INCLUDE_DIRS
        ${MP_ROOT}             # micropython/
        ${MP_ROOT}/py          # micropython/py
        ${PROJECT_DIR}         # micropython/ports/esp32
        ${MICROPY_BOARD_DIR}   # board headers (mpconfigboard.h)
        ${CMAKE_CURRENT_LIST_DIR}/..
        ${BUILD_DIR}

	INCLUDE_DIRS ${includes}
	REQUIRES ${IDF_COMPONENTS}
)

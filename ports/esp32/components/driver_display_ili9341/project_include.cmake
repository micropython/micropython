#Include all files that contain MP bindings

#Define the name of your module here
set(mod_name "ili9341")
set(driver_name "ili9341")
set(mod_register "ili9341 DISPLAY")

if(CONFIG_DRIVER_ILI9341_ENABLE)
    message(STATUS "ili9341 enabled")
    set(EXTMODS_INIT "${EXTMODS_INIT}" "\"${driver_name}\"@\"${mod_register}\"^" CACHE INTERNAL "")
else()
    message(STATUS "ili9341 disabled")
endif()
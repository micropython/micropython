#Include all files that contain MP bindings
set(mods
    "${COMPONENT_DIR}/modsndmixer.c"
)

#Define the name of your module here
set(mod_name "sndmixer")
set(mod_register "sndmixer")

if(CONFIG_DRIVER_SNDMIXER_ENABLE)
    message(STATUS "sndmixer enabled")
    set(EXTMODS "${EXTMODS}" "${mods}" CACHE INTERNAL "")
    set(EXTMODS_NAMES "${EXTMODS_NAMES}" "${mod_name}" CACHE INTERNAL "")
    #set(EXTMODS_INIT "${EXTMODS_INIT}" "\"${mod_name}\"@\"${mod_register}\"^" CACHE INTERNAL "")
else()
    message(STATUS "sndmixer disabled")
endif()
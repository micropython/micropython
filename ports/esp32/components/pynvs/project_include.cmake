#Include all files that contain MP bindings
set(mods
    "${COMPONENT_DIR}/modnvs.c"
)

#Define the name of your module here
set(mod_name "nvs")
set(mod_register "nvs")

message(STATUS "nvs enabled")
set(EXTMODS "${EXTMODS}" "${mods}" CACHE INTERNAL "")
set(EXTMODS_NAMES "${EXTMODS_NAMES}" "${mod_name}" CACHE INTERNAL "")
set(EXTMODS_INIT "${EXTMODS_INIT}" "\"${mod_name}\"@\"${mod_register}\"^" CACHE INTERNAL "")

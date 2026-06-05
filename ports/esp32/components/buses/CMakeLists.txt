#get_cmake_property(_vars VARIABLES)
#foreach(v ${_vars})
#    message(STATUS "${v} = [${${v}}]")
#endforeach()

idf_component_register(
  SRCS "buses.c"
  INCLUDE_DIRS include ${PROJECT_DIR}
    REQUIRES
        micropython
        driver
        freertos)

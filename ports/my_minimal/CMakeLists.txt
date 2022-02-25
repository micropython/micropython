cmake_minimum_required(VERSION 3.20)
#set(CMAKE_CXX_STANDARD 14)

set(MICROPY_PORT_DIR    ${CMAKE_CURRENT_SOURCE_DIR})
set(MICROPY_DIR         ${MICROPY_PORT_DIR}/../../../micropython)

# set(MP_CONFIGFILE       ${CMAKE_SOURCE_DIR}/mpconfigport.h)


# if(NOT MICROPY_DIR)
#     get_filename_component(MICROPY_DIR ${MICROPY_SOURCE_DIR}/../.. ABSOLUTE)
# endif()

set(MICROPY_TARGET app)


include(${MICROPY_DIR}/py/py.cmake)
#include(${MICROPY_DIR}/extmod/extmod.cmake)

project(my_micropython)

add_executable(app main.c)

include_directories(app 
                ${MICROPY_DIR}/py
                ${MICROPY_DIR}/shared
                )

set(MICROPY_SOURCE_PORT
        ${MICROPY_PORT_DIR}/main.c
        ${MICROPY_PORT_DIR}/mphalport.c
        )

set(MICROPY_SOURCE_SHARED
        ${MICROPY_DIR}/shared/readline/readline.c
        ${MICROPY_DIR}/shared/runtime/gchelper_generic.c
        ${MICROPY_DIR}/shared/runtime/pyexec.c
        ${MICROPY_DIR}/shared/runtime/stdout_helpers.c
        )

# add_executable(app  ${MICROPY_SOURCE_PORT} ${MICROPY_SOURCE_SHARED})

include(${MICROPY_DIR}/py/mkrules.cmake)

# set(MICROPY_SOURCE_GENERATED
#         ${MICROPY_MPVERSION}
#         ${MICROPY_MODULEDEFS}
#         ${MICROPY_QSTRDEFS_LAST}
#         ${MICROPY_QSTRDEFS_SPLIT}
#         ${MICROPY_QSTRDEFS_COLLECTED}
#         ${MICROPY_QSTRDEFS_PREPROCESSED}
#         ${MICROPY_QSTRDEFS_GENERATED}
#         )         

# target_sources(app PRIVATE
#         ${MICROPY_SOURCE_PORT}
#         ${MICROPY_SOURCE_SHARED}
#         )

#add_library(Generated ${MICROPY_SOURCE_GENERATED})

#add_library(Py ${MICROPY_SOURCE_PY})

#add_library(Port ${MICROPY_SOURCE_PORT})
#         ${MICROPY_PORT_DIR}/main.c
#         ${MICROPY_PORT_DIR}/mphalport.c
#         )

#add_library(Shared ${MICROPY_SOURCE_SHARED})
#         ${MICROPY_DIR}/shared/readline/readline.c
#         ${MICROPY_DIR}/shared/runtime/gchelper_generic.c
#         ${MICROPY_DIR}/shared/runtime/pyexec.c
#         ${MICROPY_DIR}/shared/runtime/stdout_helpers.c
#         )

#add_dependencies(Py Generated)

#target_link_libraries(app Py Generated)



# target_link_directories(app PRIVATE 
#                 ${MICROPY_DIR}/py
#                 ${MICROPY_DIR}/shared
#                 )


#include(${MICROPY_DIR}/py/mkrules.cmake)

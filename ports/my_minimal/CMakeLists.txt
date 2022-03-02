cmake_minimum_required(VERSION 3.20)
#set(CMAKE_CXX_STANDARD 14)

set(MICROPY_PORT_DIR    ${CMAKE_CURRENT_SOURCE_DIR})
set(MICROPY_DIR         ${MICROPY_PORT_DIR}/../..)
set(MICROPY_TARGET My_micropython)


project(${MICROPY_TARGET})

add_executable(${MICROPY_TARGET})

# set(MP_CONFIGFILE       ${CMAKE_SOURCE_DIR}/mpconfigport.h)


# if(NOT MICROPY_DIR)
#     get_filename_component(MICROPY_DIR ${MICROPY_SOURCE_DIR}/../.. ABSOLUTE)
# endif()




include(${MICROPY_DIR}/py/py.cmake)
include(${MICROPY_DIR}/extmod/extmod.cmake)

#project(my_micropython)



# include_directories(app 
#                 ${MICROPY_DIR}/py
#                 ${MICROPY_DIR}/shared
#                 )

# set(MICROPY_SOURCE_PORT
#         ${MICROPY_PORT_DIR}/main.c
#         ${MICROPY_PORT_DIR}/mphalport.c
#         )

set(MICROPY_SOURCE_SHARED
        readline/readline.c
        runtime/gchelper_generic.c
        runtime/pyexec.c
        runtime/stdout_helpers.c
        )

list(TRANSFORM MICROPY_SOURCE_SHARED PREPEND ${MICROPY_DIR}/shared/)

set(MICROPY_SOURCE_QSTR
        ${MICROPY_SOURCE_PY}
        ${MICROPY_SOURCE_EXTMOD}
        ${MICROPY_SOURCE_SHARED}
        #${MICROPY_SOURCE_PORT}
        )

include_directories(${MICROPY_TARGET}
        ${MICROPY_PY_DIR}
        ${MICROPY_INC_CORE}
        ${MICROPY_PORT_DIR}
        ${CMAKE_CURRENT_BINARY_DIR}
        )
# add_executable(app  ${MICROPY_SOURCE_PORT} ${MICROPY_SOURCE_SHARED})

target_sources(${MICROPY_TARGET} PRIVATE main.c mphalport.c ${MICROPY_SOURCE_QSTR})

target_link_libraries(${MICROPY_TARGET} m)

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

#target_link_libraries(app Py)



# target_link_directories(app PRIVATE 
#                 ${MICROPY_DIR}/py
#                 ${MICROPY_DIR}/shared
#                 )


#include(${MICROPY_DIR}/py/mkrules.cmake)

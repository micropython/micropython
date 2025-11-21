# This top-level micropython.cmake is responsible for listing
# the individual modules we want to include.
# Paths are absolute, and ${CMAKE_CURRENT_LIST_DIR} can be
# used to prefix subdirectories.

# Add the C example.
include(${CMAKE_CURRENT_LIST_DIR}/cexample/micropython.cmake)

# Add the CPP example.
include(${CMAKE_CURRENT_LIST_DIR}/cppexample/micropython.cmake)


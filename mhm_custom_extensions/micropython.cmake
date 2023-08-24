# This top-level micropython.cmake is responsible for listing
# the individual modules we want to include.
# Paths are absolute, and ${CMAKE_CURRENT_LIST_DIR} can be
# used to prefix subdirectories.

# Add the analogue readings module
include(${CMAKE_CURRENT_LIST_DIR}/analogue_readings/micropython.cmake)

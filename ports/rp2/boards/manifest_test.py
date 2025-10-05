# Manifest for testing the build with RPI_PICO_W configuration.

# Include RPI_PICO_W board manifest.
include("$(PORT_DIR)/boards/RPI_PICO_W/manifest.py")

# Test user C modules (for CI only).
c_module("$(MPY_DIR)/examples/usercmodule/cexample")
c_module("$(MPY_DIR)/examples/usercmodule/cppexample")

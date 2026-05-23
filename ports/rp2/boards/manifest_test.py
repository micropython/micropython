# Manifest for testing the c_module() build feature.

# Include the default board manifest.
include("manifest.py")

# Test user C modules via c_module().
c_module("$(MPY_DIR)/examples/usercmodule/cexample")
c_module("$(MPY_DIR)/examples/usercmodule/cppexample")
c_module("$(MPY_DIR)/examples/usercmodule/subpackage")

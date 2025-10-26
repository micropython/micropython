# Manifest for testing the build.

# Include standard manifest.
include("manifest.py")

# Test user C modules.
c_module("$(MPY_DIR)/examples/usercmodule/cexample")
c_module("$(MPY_DIR)/examples/usercmodule/cppexample")
c_module("$(MPY_DIR)/examples/usercmodule/subpackage")

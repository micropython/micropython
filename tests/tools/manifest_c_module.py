# Manifest for testing the c_module() build feature.
#
# Shared across ports; used in CI to exercise the c_module() manifest entry
# alongside per-port builds that also test the legacy USER_C_MODULES=
# command-line variable.

# Include the port's default board manifest.
include("$(PORT_DIR)/boards/manifest.py")

# Test user C modules via c_module().
c_module("$(MPY_DIR)/examples/usercmodule/cexample")
c_module("$(MPY_DIR)/examples/usercmodule/cppexample")
c_module("$(MPY_DIR)/examples/usercmodule/subpackage")

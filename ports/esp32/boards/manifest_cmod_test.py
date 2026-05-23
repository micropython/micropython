# Manifest for testing the c_module() build feature.
#
# Sibling file manifest_test.py covers the USER_C_MODULES= command-line test;
# this one exists separately so the two CI builds use different manifests and
# don't end up declaring the same example C modules from both sources.

# Include the default board manifest.
include("manifest.py")

# Test user C modules via c_module().
c_module("$(MPY_DIR)/examples/usercmodule/cexample")
c_module("$(MPY_DIR)/examples/usercmodule/cppexample")
c_module("$(MPY_DIR)/examples/usercmodule/subpackage")

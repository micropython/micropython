add_library("unix-ffi", "$(MPY_LIB_DIR)/unix-ffi")
freeze_as_str("frzstr")
freeze_as_mpy("frzmpy")
freeze_mpy("$(MPY_DIR)/tests/frozen")
require("ssl")

# Include example user C modules.
c_module("$(MPY_DIR)/examples/usercmodule/cexample")
c_module("$(MPY_DIR)/examples/usercmodule/cppexample")

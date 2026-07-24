add_library("unix-ffi", "$(MPY_LIB_DIR)/unix-ffi")
freeze_as_str("frzstr")
freeze_as_mpy("frzmpy")
freeze_mpy("$(MPY_DIR)/tests/assets")
require("ssl")

# used for typing runtime tests
require("abc")
require("bundle-typing", extensions=True)

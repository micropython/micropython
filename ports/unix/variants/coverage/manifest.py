add_library("unix-ffi", "$(MPY_LIB_DIR)/unix-ffi")
freeze_as_str("frzstr")
freeze_as_mpy("frzmpy")
freeze_mpy("$(MPY_DIR)/tests/frozen")
require("ssl")
# T-string support: string package from extmod
include("$(MPY_DIR)/extmod/string/manifest.py")

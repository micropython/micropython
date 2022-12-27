freeze_as_str("frzstr")
freeze_as_mpy("frzmpy")
freeze_mpy("$(MPY_DIR)/tests/frozen")
module("ssl.py", base_path="$(MPY_DIR)/extmod/ssl", opt=3)

include("$(PORT_DIR)/boards/manifest.py")
include("$(MPY_DIR)/user_modules/lv_binding_micropython/ports/esp32")
module("st77xx.py", base_path="$(MPY_DIR)/user_modules/lv_binding_micropython/driver/generic")
module("xpt2046.py", base_path="$(MPY_DIR)/user_modules/lv_binding_micropython/driver/generic")

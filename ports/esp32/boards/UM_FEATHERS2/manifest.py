include("$(PORT_DIR)/boards/manifest.py")
module("dotstar.py", base_path="$(PORT_DIR)/boards/UM_TINYPICO/modules", opt=3)
freeze("modules")

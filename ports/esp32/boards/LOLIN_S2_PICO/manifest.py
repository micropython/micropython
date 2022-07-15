include("$(PORT_DIR)/boards/manifest.py")
freeze("./modules")

include("$(MPY_DIR)/drivers/display", ssd1306=True)

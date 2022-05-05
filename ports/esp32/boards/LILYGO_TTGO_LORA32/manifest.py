include("$(PORT_DIR)/boards/manifest.py")
freeze("modules")

freeze("$(MPY_DIR)/drivers/display", "ssd1306.py")

include("$(PORT_DIR)/boards/manifest.py", extra_drivers=False)
freeze("$(MPY_DIR)/drivers/display", "ssd1306.py")

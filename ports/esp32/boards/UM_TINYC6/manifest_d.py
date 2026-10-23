include("$(PORT_DIR)/boards/manifest.py")
freeze("$(BOARD_DIR)/modules", ("tinyc6d.py", "max17048.py", "fxl6408.py"))

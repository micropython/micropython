include("$(PORT_DIR)/boards/manifest.py")
freeze("$(BOARD_DIR)/modules", ("edges3d.py", "max17048.py", "fxl6408.py"))

include("$(PORT_DIR)/boards/manifest.py")
freeze("$(BOARD_DIR)/modules", ("feathers3d.py", "max17048.py"))

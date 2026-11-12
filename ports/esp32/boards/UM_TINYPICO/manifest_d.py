include("$(PORT_DIR)/boards/manifest.py")
freeze("$(BOARD_DIR)/modules", ("tinypicod.py", "max17048.py"))

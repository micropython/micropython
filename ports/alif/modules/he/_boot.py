# Change working directory to ROMFS, so boot.py and main.py can run from there.
try:
    import os

    os.chdir("/rom")
except:
    pass

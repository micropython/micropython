# tests meminfo functions in micropython module

try:
    import micropython

    micropython.mem_info
except (ImportError, AttributeError):
    print("SKIP")
    raise SystemExit

micropython.mem_info()
micropython.mem_info(1)
micropython.qstr_info()
micropython.qstr_info(1)

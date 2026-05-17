try:
    import platform
except ImportError:
    print("SKIP")
    raise SystemExit

print(type(platform.python_compiler()))
print(type(platform.libc_ver()))

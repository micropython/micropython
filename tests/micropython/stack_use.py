# tests stack_use function in micropython module

try:
    import micropython

    micropython.stack_use
except (ImportError, AttributeError):
    print("SKIP")
    raise SystemExit

print(type(micropython.stack_use()))  # output varies

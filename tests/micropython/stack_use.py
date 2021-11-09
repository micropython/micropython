# tests stack_use function in micropython module
from src import micropython

if not hasattr(micropython, "stack_use"):
    print("SKIP")
else:
    print(type(micropython.stack_use()))  # output varies

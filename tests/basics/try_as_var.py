try:
    raise ValueError(534)
except ValueError as e:
    print(repr(e))

# Var bound in except block is automatically deleted
try:
    e
except NameError:
    print("NameError")

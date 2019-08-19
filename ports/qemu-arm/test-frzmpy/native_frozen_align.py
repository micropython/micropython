import micropython

@micropython.native
def native_x(x):
    print(x + 1)

@micropython.native
def native_y(x):
    print(x + 1)

@micropython.native
def native_z(x):
    print(x + 1)

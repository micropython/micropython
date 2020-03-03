import micropython as micropython

# check we can get and set the level
micropython.opt_level(0)
print(micropython.opt_level())
micropython.opt_level(1)
print(micropython.opt_level())

# check that the optimisation levels actually differ
micropython.opt_level(0)
exec('print(__debug__)')
micropython.opt_level(1)
exec('print(__debug__)')
exec('assert 0')

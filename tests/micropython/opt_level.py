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

# check that level 3 doesn't store line numbers
# the expected output is that any line is printed as "line 1"
micropython.opt_level(3)
exec('try:\n xyz\nexcept NameError as er:\n import sys\n sys.print_exception(er)')

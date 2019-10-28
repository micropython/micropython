import micropython as micropython

# check that level 3 doesn't store line numbers
# the expected output is that any line is printed as "line 1"
micropython.opt_level(3)
exec('try:\n xyz\nexcept NameError as er:\n import sys\n sys.print_exception(er)')

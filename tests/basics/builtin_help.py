# test builtin help function

try:
    help
except NameError:
    print("SKIP")
    raise SystemExit

help() # no args
help(help) # help for a function
help(int) # help for a class
help(1) # help for an instance
import micropython
help(micropython) # help for a module
help('modules') # list available modules

class A:
    x = 1
    y = 2
    del x
help(A)

print('done') # so last bit of output is predictable

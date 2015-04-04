# test builtin dir

# dir of locals
print('__name__' in dir())

# dir of module
import sys
print('platform' in dir(sys))


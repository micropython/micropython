print("time from filesystem")

# Tests the CPython-compatible / non-u-prefix way of forcing a builtin
# import.
import sys

_path = sys.path
sys.path = ()
from time import *

sys.path = _path
del _path

extra = 1

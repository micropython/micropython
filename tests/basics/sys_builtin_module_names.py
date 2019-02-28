# test sys.builtin_module_names

import sys
try:
    sys.builtin_module_names
except AttributeError:
    print('SKIP')
    raise SystemExit

# common modules to all ports + CPython
print('sys' in sys.builtin_module_names)
print('gc' in sys.builtin_module_names)
print('builtins' in sys.builtin_module_names)

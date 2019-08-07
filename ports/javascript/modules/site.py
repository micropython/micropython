import sys
import builtins

for name in ("sys", "__main__", "builtins"):
    sys.modules[name] = __import__(name)

import imp

builtins.imp = imp


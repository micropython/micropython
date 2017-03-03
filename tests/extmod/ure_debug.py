# test printing debugging info when compiling
try:
    import ure
except ImportError:
    import sys
    print("SKIP")
    sys.exit()

ure.compile('^a|b[0-9]\w$', ure.DEBUG)

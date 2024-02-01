# Test MicroPython-specific behaviour of tls.SSLContext.

try:
    import tls
except ImportError:
    print("SKIP")
    raise SystemExit

# Test constructing without any arguments (in CPython it's a DeprecationWarning).
try:
    tls.SSLContext()
except TypeError:
    print("TypeError")

# Test attributes that don't exist (in CPython new attributes can be added).
# This test is needed for coverage because SSLContext implements a custom attr handler.
ctx = tls.SSLContext(tls.PROTOCOL_TLS_CLIENT)
try:
    ctx.does_not_exist
except AttributeError:
    print("AttributeError on load")
try:
    ctx.does_not_exist = None
except AttributeError:
    print("AttributeError on store")
try:
    del ctx.does_not_exist
except AttributeError:
    print("AttributeError on delete")

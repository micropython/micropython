# Test MicroPython-specific behaviour of ssl.SSLContext.

try:
    import ssl
except ImportError:
    print("SKIP")
    raise SystemExit

# Test constructing without any arguments (in CPython it's a DeprecationWarning).
try:
    ssl.SSLContext()
except TypeError:
    print("TypeError")

# Test attributes that don't exist (in CPython new attributes can be added).
# This test is needed for coverage because SSLContext implements a custom attr handler.
ctx = ssl.SSLContext(ssl.PROTOCOL_TLS_CLIENT)
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

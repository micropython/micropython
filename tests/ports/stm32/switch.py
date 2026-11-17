# Test pyb.Switch() basic behaviour.

try:
    from pyb import Switch
except ImportError:
    print("SKIP")
    raise SystemExit

sw = Switch()
print(sw())
sw.callback(print)
sw.callback(None)

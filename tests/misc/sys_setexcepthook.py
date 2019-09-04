import sys
try:
    sys.setexcepthook
except:
    print("SKIP")
    raise SystemExit

def handler(t, v, bt):
    print("type:", str(t))
    print("value:", str(v))

sys.setexcepthook(handler)

try:
    raise Exception("inside except block")
except Exception:
    pass

raise Exception("outside except block")

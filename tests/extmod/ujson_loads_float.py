try:
    import ujson as json
except ImportError:
    try:
        import json
    except ImportError:
        print("SKIP")
        raise SystemExit

def my_print(o):
    print('%.3f' % o)

my_print(json.loads('1.2'))
my_print(json.loads('1e2'))
my_print(json.loads('-2.3'))
my_print(json.loads('-2e3'))
my_print(json.loads('-2e-3'))

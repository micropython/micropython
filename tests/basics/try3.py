# nested exceptions

def f():
    try:
        foo()
    except Exception:
        print("except 1")
        try:
            baz()
        except Exception:
            print("except 2")
        bar()

try:
    f()
except Exception:
    print("f except")

# triple nested exceptions

def f():
    try:
        foo()
    except Exception:
        print("except 1")
        try:
            bar()
        except Exception:
            print("except 2")
            try:
                baz()
            except Exception:
                print("except 3")
            bak()

try:
    f()
except Exception:
    print("f except")

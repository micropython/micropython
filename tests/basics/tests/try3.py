# nested exceptions

def f():
    try:
        foo()
    except:
        print("except 1")
        try:
            baz()
        except:
            print("except 2")
        bar()

try:
    f()
except:
    print("f except")

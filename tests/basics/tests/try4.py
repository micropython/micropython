# triple nested exceptions

def f():
    try:
        foo()
    except:
        print("except 1")
        try:
            bar()
        except:
            print("except 2")
            try:
                baz()
            except:
                print("except 3")
            bak()

try:
    f()
except:
    print("f except")

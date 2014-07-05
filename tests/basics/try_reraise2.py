# Reraise not the latest occured exception
def f():
    try:
        raise ValueError("val", 3)
    except:
        try:
            print(1)
            raise TypeError
        except:
            print(2)
            try:
                print(3)
                try:
                    print(4)
                    raise AttributeError
                except:
                    print(5)
                    pass
                print(6)
                raise
            except TypeError:
                print(7)
                pass
            print(8)
        print(9)
        # This should raise original ValueError, not the most recently occurred AttributeError
        raise

try:
    f()
except ValueError as e:
    print(repr(e))

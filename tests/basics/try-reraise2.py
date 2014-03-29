# Reraise not the latest occured exception
def f():
    try:
        raise ValueError("val", 3)
    except:
        try:
            raise TypeError
        except:
            try:
                try:
                    raise AttributeError
                except:
                    pass
                raise
            except TypeError:
                pass
        # This should raise original ValueError, not the most recently occurred AttributeError
        raise

try:
    f()
except ValueError as e:
    print(repr(e))

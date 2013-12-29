def f():
    try:
        f()
    except:
        g()
    finally:
        f()

    try:
        f()
    except Exception:
        g()
    finally:
        f()

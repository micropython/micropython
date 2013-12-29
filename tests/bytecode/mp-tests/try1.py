def f(x):
    try:
        f(x)
    except:
        f(x)
    try:
        f(x)
    except Exception:
        f(x)
    try:
        f(x)
    except Exception as e:
        f(x, e)

def __getattr__(attr):
    print("__getattr__", attr)
    raise ValueError(attr)

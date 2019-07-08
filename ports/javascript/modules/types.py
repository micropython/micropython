import sys


# not thread safe

def ModuleType(name):
    if sys.modules.get('name'):
        print("Error : module %s exists !"%name)
        return sys.modules[name]

    # get a new fresh module
    import imp_empty_pivot_module as pivot
    # low risk, who would call his module like that ?
    del sys.modules['imp_empty_pivot_module']

    #still unknown at this time
    del pivot.__file__

    pivot.__name__ = name

    return pivot

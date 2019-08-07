import sys
import imp


# not thread safe

def ModuleType(name):
    if sys.modules.get('name'):
        print("Error : module %s exists !"%name)
        return sys.modules[name]

    # get a new fresh module
    # be sure to use the builtin func
    pivot = imp.__import__('imp_empty_pivot_module') #
    # low risk, who would call his module like that ?
    del sys.modules['imp_empty_pivot_module']

    #still unknown at this time
    if hasattr(pivot,'__file__'):
        del pivot.__file__

    pivot.__name__ = name

    return pivot

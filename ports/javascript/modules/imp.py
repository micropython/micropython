import sys
import builtins
import types

# keep the builtin function accessible in this module and via imp.__import__
__import__ = __import__

# Deprecated since version 3.4: Use types.ModuleType instead.
# but micropython aims toward full 3.4

# Return a new empty module object called name. This object is not inserted in sys.modules.
def new_module(name):
    return types.ModuleType(name)


# not spaghetti
def importer(name,*argv,**kw):
    global __import__
    try:
        return __import__(name,*argv)
    except ImportError:
        pass

    file = ':{0}.py'.format(name)
    print("INFO: getting online version of",file)
    # todo open the file via open() or raise importerror
    try:
        code = open(file,'r').read()
    except:
        raise ImportError('module not found')

    #build a empty module
    mod = types.ModuleType(name)

    mod.__file__ = file

    # compile module from cached file
    try:
        code = compile( code, file, 'exec')
    except Exception as e:
        sys.print_exception(e)
        raise

    # execute it in its own empty namespace.
    try:
        ns = vars(mod)
    except:
        print("WARNING: this python implementation lacks vars()")
        ns = mod.__dict__


    try:
        exec( code, ns, ns)
    except Exception as e:
        sys.print_exception(e)
        raise

    # though micropython would normally insert module before executing the whole body
    # do it after.
    sys.modules[name] = mod
    return mod

# install hook
builtins.__import__ = importer
print("__import__ is now", importer)

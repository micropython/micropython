import sys
import builtins
import types

# Deprecated since version 3.4: Use types.ModuleType instead.
# but micropython aims toward full 3.4

# Return a new empty module object called name. This object is not inserted in sys.modules.
def new_module(name):
    return types.ModuleType(name)



def load_module(module, *argv):
    m = new_module(module)
    if m:
        import embed
        file = '/assets/%s.py' % module
        mroot = module.split('.')[0]
        m = sys.modules[mroot]
        runf(file, module=embed.vars(m), patch='\n\n__file__=%r\n' % file )
        globals()[mroot] = m
        return m

try:
    vars
except:
    #wasm port
    import embed
    builtins.vars = embed.vars

try:
    vars
except:
    print(""" This could provide a workaround for :\r
    https://github.com/pmp-p/micropython-ports-wasm/issues/5\r
use imp.load_module(modulename) to load modules from /assets/*.py
""")
    raise ImportError("ERROR: this build has no vars() support at all")


# keep the builtin function accessible in this module and from imp.__import__
__import__ = __import__

def importer(name,*argv):
    global __import__
    if sys.modules.get(name,None) is None:
        print("import %s" % name,argv)
    try:
        return __import__(name,*argv)
    except ImportError:
        pass


    file = ':{0}.py'.format(name)
    print("trying to go online for",file)
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
    ns = vars(mod)

    try:
        exec( code, ns, ns)
    except Exception as e:
        sys.print_exception(e)
        raise

    # though micropython would normally insert module before executing the whole body
    # do it after.
    sys.modules[name] = mod
    return mod



def reload(name):
    if sys.modules.get(name,None):
        del sys.modules[name]
    return importError(name)


# install hook
builtins.__import__ = importer
print("__import__ is now", importer)

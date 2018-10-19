
# test __getattr__ on module
this = __import__(__name__)
try:
    this.does_not_exist
except AttributeError:
    print('AttributeError')

def __getattr__(attr):
    if attr=='does_not_exist':
        return False
    raise AttributeError 

assert this.does_not_exist is False, '__getattr__ on module failed'



# test passing a user-defined mapping as the argument to **

def foo(**kw):
    print(sorted(kw.items()))

class Mapping:
    def keys(self):
        return ['a', 'b', 'c']

    def __getitem__(self, key):
        if key == 'a':
            return 1
        else:
            return 2

foo(**Mapping())

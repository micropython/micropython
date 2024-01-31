class a:
    def __init__(self):
        self.d = {}

    def __setitem__(self, k, v):
        print("a", k, v)
        self.d[k] = v

    def __getitem__(self, k):
        return self.d[k]

class b(a):
    def __setitem__(self, k, v):
        print("b", k, v)
        super().__setitem__(k, v)

b1 = b()
b1[1] = 2
print(b1[1])

class mydict(dict):
    def __setitem__(self, k, v):
        print(k, v)
        super().__setitem__(k, v)

d = mydict()
d[3] = 4
print(d[3])
print(d.keys())

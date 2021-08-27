"""
test that calling super() getter property in subclass will return the value
"""


class A:
    @property
    def p(self):
        return {"a": 10}


class AA(A):
    @property
    def p(self):
        return super().p


a = AA()
print(a.p)

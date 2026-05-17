# test that no-arg super() works when self is closed over

class A:
    def __init__(self):
        self.val = 4
    def foo(self):
        # we access a member of self to check that self is correct
        return list(range(self.val))
class B(A):
    def foo(self):
        # self is closed over because it's referenced in the list comprehension
        # and then super() must detect this and load from the closure cell
        return [self.bar(i) for i in super().foo()]
    def bar(self, x):
        return 2 * x

print(A().foo())
print(B().foo())

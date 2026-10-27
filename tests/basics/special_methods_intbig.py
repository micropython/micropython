# Test class special methods, that use a bigint.
# requires: int-big

class A:
    def __int__(self):
        return 1 << 100


print(int(A()))

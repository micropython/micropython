# test builtin hash function

print({1 << 66:1}) # hash big int
print({-(1 << 66):2}) # hash negative big int

# __hash__ returning a large number should be truncated
class F:
    def __hash__(self):
        return 1 << 70 | 1
print(hash(F()) != 0)

# this had a particular error with internal integer arithmetic of hash function
print(hash(6699999999999999999999999999999999999999999999999999999999999999999999) != 0)

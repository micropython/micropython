s = {1}
print(s.pop())
try:
    print(s.pop(), "!!!")
except KeyError:
    pass
else:
    print("Failed to raise KeyError")

# this tests an optimisation in mp_set_remove_first
# N must not be equal to one of the values in hash_allocation_sizes
N = 11
s = set(range(N))
while s:
    print(s.pop()) # last pop() should trigger the optimisation
for i in range(N):
    s.add(i) # check that we can add the numbers back to the set
print(sorted(s))

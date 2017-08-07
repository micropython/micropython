# test builtin min and max functions
try:
    min
    max
except:
    print("SKIP")
    raise SystemExit

print(min(0,1))
print(min(1,0))
print(min(0,-1))
print(min(-1,0))

print(max(0,1))
print(max(1,0))
print(max(0,-1))
print(max(-1,0))

print(min([1,2,4,0,-1,2]))
print(max([1,2,4,0,-1,2]))

# test with key function
lst = [2, 1, 3, 4]
print(min(lst, key=lambda x:x))
print(min(lst, key=lambda x:-x))
print(min(1, 2, 3, 4, key=lambda x:-x))
print(min(4, 3, 2, 1, key=lambda x:-x))
print(max(lst, key=lambda x:x))
print(max(lst, key=lambda x:-x))
print(max(1, 2, 3, 4, key=lambda x:-x))
print(max(4, 3, 2, 1, key=lambda x:-x))

# need at least 1 item in the iterable
try:
    min([])
except ValueError:
    print("ValueError")

# 'default' tests
print(min([1, 2, 3, 4, 5], default=-1))
print(min([], default=-1))
print(max([1, 2, 3, 4, 5], default=-1))
print(max([], default=-1))
# make sure it works with lazy iterables
# can't use Python generators here, as they're not supported
# byy native codegenerator.
print(min(enumerate([]), default=-10))

a = iter([0, 1])
print(next(a))
print(next(a))
try:
    next(a)
except StopIteration:
    print('end')

try:
    next((x for x in [] if x))
except StopIteration:
    print('end')

print(next((x for x in [] if x), 'end'))

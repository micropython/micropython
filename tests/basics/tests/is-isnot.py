print(1 is 1)
print(1 is 2)
print(1 is not 1)
print(1 is not 2)


print([1, 2] is [1, 2])
a = [1, 2]
b = a
print(b is a)

# TODO: strings require special "is" handling, postponed
# until qstr refactor.
#print("a" is "a")

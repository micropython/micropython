# test % operation on big integers

delta = 100000000000000000000000000000012345

for i in range(11):
    for j in range(11):
        x = delta * (i)# - 5) # TODO reinstate negative number test when % is working with sign correctly
        y = delta * (j)# - 5) # TODO reinstate negative number test when % is working with sign correctly
        if y != 0:
            print(x % y)

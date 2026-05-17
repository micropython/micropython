while True:
    break

for i in range(4):
    print('one', i)
    if i > 2:
        break
    print('two', i)

for i in [1, 2, 3, 4]:
    if i == 3:
        break
    print(i)

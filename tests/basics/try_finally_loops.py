# Test various loop types, some may be implemented/optimized differently
while True:
    try:
        break
    finally:
        print('finally 1')


for i in [1, 5, 10]:
    try:
        continue
    finally:
        print('finally 2')

for i in range(3):
    try:
        continue
    finally:
        print('finally 3')

# Multi-level
for i in range(4):
    print(i)
    try:
        while True:
            try:
                try:
                    break
                finally:
                    print('finally 1')
            finally:
                print('finally 2')
        print('here')
    finally:
        print('finnaly 3')

# break from within try-finally, within for-loop
for i in [1]:
    try:
        print(i)
        break
    finally:
        print('finally 4')

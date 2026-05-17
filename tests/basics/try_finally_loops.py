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

# Test unwind-jump where there is nothing in the body of the try or finally.
# This checks that the bytecode emitter allocates enough stack for the unwind.
for i in [1]:
    try:
        break
    finally:
        pass

# The following test checks that the globals dict is valid after a call to a
# function that has an unwind jump.
# There was a bug where an unwind jump would trash the globals dict upon return
# from a function, because it used the Python-stack incorrectly.
def f():
    for i in [1]:
        try:
            break
        finally:
            pass
def g():
    global global_var
    f()
    print(global_var)
global_var = 'global'
g()

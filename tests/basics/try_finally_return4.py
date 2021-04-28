# test try-finally with return, where unwinding return has to go through
# another try-finally which may affect the behaviour of the return

# case where a simple try-finally executes during an unwinding return
def f(x):
    try:
        try:
            if x:
                return 42
        finally:
            try:
                print(1)
            finally:
                print(2)
            print(3)
        print(4)
    finally:
        print(5)
print(f(0))
print(f(1))

# case where an unwinding return is replaced by another one
def f(x):
    try:
        try:
            if x:
                return 42
        finally:
            try:
                print(1)
                return 43
            finally:
                print(2)
            print(3)
        print(4)
    finally:
        print(5)
print(f(0))
print(f(1))

# case where an unwinding return is cancelled by an exception
def f(x):
    try:
        try:
            if x:
                return 42
        finally:
            try:
                print(1)
                raise ValueError # cancels any active return
            finally:
                print(2)
            print(3)
        print(4)
    finally:
        print(5)
try:
    print(f(0))
except:
    print('caught')
try:
    print(f(1))
except:
    print('caught')

# case where an unwinding return is cancelled then resumed
def f(x):
    try:
        try:
            if x:
                return 42
        finally:
            try:
                print(1)
                raise Exception # cancels any active return
            except: # cancels the exception and resumes any active return
                print(2)
            print(3)
        print(4)
    finally:
        print(5)
print(f(0))
print(f(1))

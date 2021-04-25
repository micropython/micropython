# test 'return' within the finally block, with nested finally's
# only inactive finally's should be executed, and only once

# basic nested finally's, the print should only be executed once
def f():
    try:
        raise TypeError
    finally:
        print(1)
        try:
            raise ValueError
        finally:
            return 42
print(f())

# similar to above but more nesting
def f():
    try:
        raise ValueError
    finally:
        print(1)
        try:
            raise TypeError
        finally:
            print(2)
            try:
                pass
            finally:
                return 42
print(f())

# similar to above but even more nesting
def f():
    try:
        raise ValueError
    finally:
        print(1)
        try:
            raise TypeError
        finally:
            print(2)
            try:
                raise Exception
            finally:
                print(3)
                return 42
print(f())

# upon return some try's are active, some finally's are active, some inactive
def f():
    try:
        try:
            pass
        finally:
            print(2)
            return 42
    finally:
        print(1)
print(f())

# same as above but raise instead of pass
def f():
    try:
        try:
            raise ValueError
        finally:
            print(2)
            return 42
    finally:
        print(1)
print(f())

# upon return exception stack holds: active finally, inactive finally, active finally
def f():
    try:
        raise Exception
    finally:
        print(1)
        try:
            try:
                pass
            finally:
                print(3)
                return 42
        finally:
            print(2)
print(f())

# same as above but raise instead of pass in innermost try block
def f():
    try:
        raise Exception
    finally:
        print(1)
        try:
            try:
                raise Exception
            finally:
                print(3)
                return 42
        finally:
            print(2)
print(f())

def foo(x):
    for i in range(x):
        try:
            pass
        finally:
            try:
                try:
                    print(x, i)
                finally:
                    try:
                        1 / 0
                    finally:
                        return 42
            finally:
                print('continue')
                continue
print(foo(4))

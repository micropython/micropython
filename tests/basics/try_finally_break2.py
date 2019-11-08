def foo(x):
    for i in range(x):
        for j in range(x):
            try:
                print(x, i, j, 1)
            finally:
                try:
                    try:
                        print(x, i, j, 2)
                    finally:
                        try:
                            1 / 0
                        finally:
                            print(x, i, j, 3)
                            break
                finally:
                    print(x, i, j, 4)
                    break
print(foo(4))

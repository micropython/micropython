# test native try handling

# deeply nested try (9 deep)
@micropython.native
def f():
    try:
        try:
            try:
                try:
                    try:
                        try:
                            try:
                                try:
                                    try:
                                        raise ValueError
                                    finally:
                                        print(8)
                                finally:
                                    print(7)
                            finally:
                                print(6)
                        finally:
                            print(5)
                    finally:
                        print(4)
                finally:
                    print(3)
            finally:
                print(2)
        finally:
            print(1)
    except ValueError:
        print('ValueError')
f()

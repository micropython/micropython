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

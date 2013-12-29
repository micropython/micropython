# test when a function has cell and free vars

def f():
    f_local = 1
    f_cell = 2

    def g():
        g_local = 3
        g_cell = f_cell + 4

        def h():
            h1_local = 4
            h2_local = f_cell + g_cell

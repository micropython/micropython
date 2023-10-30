import io


def test_truncate(file):
    file.write("a" * 64)

    # arg < 0
    try:
        file.truncate(-1)
    except Exception as e:
        # Usually this is OSError, but it is ValueError for
        # a StringIO in CPython.
        print("-1 invalid")

    # no arg
    file.seek(56)
    print(file.truncate())  # 56
    print(file.seek(0, 2))  # 56

    # position < arg < size: file gets shorter
    file.seek(40)
    print(file.truncate(48))  # 48
    print(file.seek(0, 2))  # 48

    # arg < position < size: file gets shorter, position decreases
    file.seek(40)
    print(file.truncate(32))  # 32
    print(file.seek(0, 2))  # 32
    print(file.tell())  # 32

    # position < size < arg: file gets longer, unless it is StringIO
    file.seek(24)
    print(file.truncate(64))  # 64
    print(file.seek(0, 2))  # 64 or 32


with open("f", "w+") as file:
    test_truncate(file)

test_truncate(io.StringIO())

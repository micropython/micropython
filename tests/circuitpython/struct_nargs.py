import struct


def do_pack(*args):
    try:
        print(struct.pack(*args))
    except Exception as e:
        print("ERROR")


do_pack("H")
do_pack("H", 1)
do_pack("H", 1, 2)

do_pack("2H")
do_pack("2H", 1)
do_pack("2H", 1, 2)

do_pack("3H")
do_pack("3H", 1)
do_pack("3H", 1, 2)
do_pack("3H", 1, 2, 3)
do_pack("3H", 1, 2, 3, 4)

do_pack("4sH", b"x")
do_pack("4sH", b"x", 1)
do_pack("4sH", b"x", 1, 2)

do_pack("4s2H", b"x")
do_pack("4s2H", b"x", 1)
do_pack("4s2H", b"x", 1, 2)
do_pack("4s2H", b"x", 1, 2, 3)

do_pack("4s3H", b"x")
do_pack("4s3H", b"x", 1)
do_pack("4s3H", b"x", 1, 2)
do_pack("4s3H", b"x", 1, 2, 3)
do_pack("4s3H", b"x", 1, 2, 3, 4)

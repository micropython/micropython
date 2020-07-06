try:
    from ucryptolib import aes
except ImportError:
    print("SKIP")
    raise SystemExit


def _new(k, ctr_initial):
    return aes(k, 6, ctr_initial)


try:
    _new(b"x" * 16, b"x" * 16)
except ValueError as e:
    # is CTR support disabled?
    if e.args[0] == "mode":
        print("SKIP")
        raise SystemExit
    raise e

crypto = _new(b"1234" * 4, b"5678" * 4)
enc = crypto.encrypt(b"a")
print(enc)
enc += crypto.encrypt(b"b" * 1000)
print(enc)

crypto = _new(b"1234" * 4, b"5678" * 4)
print(crypto.decrypt(enc))

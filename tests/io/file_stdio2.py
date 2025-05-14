# Test sys.std*.buffer objects.

import sys

try:
    sys.stdout.buffer
    sys.stdin.buffer
    sys.stderr.buffer
except AttributeError:
    print("SKIP")
    raise SystemExit


# force cpython to flush after every print
# this is to sequence stdout and stderr
def print_flush(*args, **kwargs):
    try:
        print(*args, **kwargs, flush=True)
    except TypeError:
        print(*args, **kwargs)


print_flush("==stdin==")
print_flush(sys.stdin.buffer.fileno())


print_flush("==stdout==")
print_flush(sys.stdout.buffer.fileno())
n_text = sys.stdout.write("The quick brown fox jumps over the lazy dog\n")
sys.stdout.flush()
n_binary = sys.stdout.buffer.write("The quick brown fox jumps over the lazy dog\n".encode("utf-8"))
sys.stdout.buffer.flush()
print_flush("n_text:{} n_binary:{}".format(n_text, n_binary))

# temporarily disabling unicode tests until future PR which fixes unicode write character count
# n_text = sys.stdout.write("🚀")
# sys.stdout.flush()
# n_binary = sys.stdout.buffer.write("🚀".encode("utf-8"))
# sys.stdout.buffer.flush()
# print_flush("")
# print_flush("n_text:{} n_binary:{}".format(n_text, n_binary))
# n_text = sys.stdout.write("1🚀2a3α4b5β6c7γ8d9δ0ぁ1🙐")
# sys.stdout.flush()
# n_binary = sys.stdout.buffer.write("1🚀2a3α4b5β6c7γ8d9δ0ぁ1🙐".encode("utf-8"))
# sys.stdout.buffer.flush()
# print_flush("")
# print_flush("n_text:{} n_binary:{}".format(n_text, n_binary))


print_flush("==stderr==")
print_flush(sys.stderr.buffer.fileno())
n_text = sys.stderr.write("The quick brown fox jumps over the lazy dog\n")
sys.stderr.flush()
n_binary = sys.stderr.buffer.write("The quick brown fox jumps over the lazy dog\n".encode("utf-8"))
sys.stderr.buffer.flush()
print_flush("n_text:{} n_binary:{}".format(n_text, n_binary))

# temporarily disabling unicode tests until future PR which fixes unicode write character count
# n_text = sys.stderr.write("🚀")
# sys.stderr.flush()
# n_binary = sys.stderr.buffer.write("🚀".encode("utf-8"))
# sys.stderr.buffer.flush()
# print_flush("")
# print_flush("n_text:{} n_binary:{}".format(n_text, n_binary))
# print_flush("")

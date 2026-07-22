import sys

try:
    sys._getframe
except AttributeError:
    print("SKIP")
    raise SystemExit

top_frame = sys._getframe()

print(top_frame.f_code.co_name == "<module>")


def new_frame():
    curr_frame = sys._getframe()
    prev_frame = sys._getframe(1)

    print(curr_frame.f_code.co_name == "new_frame")

    print(prev_frame.f_code.co_name == "<module>")
    print(curr_frame.f_back.f_code.co_name == "<module>")

    print(prev_frame.f_lineno == curr_frame.f_back.f_lineno)
    print(prev_frame.f_code.co_filename == curr_frame.f_back.f_code.co_filename)


new_frame()

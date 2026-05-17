# test handling of unicode chars in format strings

print("α".format())
print("{α}".format(α=1))

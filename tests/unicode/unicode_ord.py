# test builtin ord with unicode characters

print(ord("α"))


class StrSubclass(str):
    pass


print(ord(StrSubclass("é")))

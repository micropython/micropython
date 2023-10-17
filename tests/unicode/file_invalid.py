try:
    f = open("unicode/data/utf-8_invalid.txt", encoding="utf-8")
    f.read()
except UnicodeError:
    print("UnicodeError")

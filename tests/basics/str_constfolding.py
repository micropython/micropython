# tests str constant folding in compiler

# single str
print("")
print("Hello")

# multiple strs concatenation
print("Hello" + ",")
print("Hello" + "," + " ")
print("Hello" + "," + " " + "World!")

# mixed str non-str concatenation
print("Hello" + chr(44))
print("Hello" + chr(44) + " ")
print("Hello" + chr(44) + chr(32))
print("Hello" + chr(44) + " " + "World!")
print("Hello" + ", {}".format("World") + "!")
print("Hello, World!" + str(2))
print("Hello, World!" + str(2))

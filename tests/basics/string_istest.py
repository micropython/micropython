print("".isspace())
print(" \t\n\r\v\f".isspace())
print("a".isspace())
print(" \t\n\r\v\fa".isspace())
print("".isalpha())
print("abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ".isalpha())
print("0abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ".isalpha())
print("this ".isalpha())
print("".isdigit())
print("0123456789".isdigit())
print("0123456789a".isdigit())
print("0123456789 ".isdigit())
print("".isupper())
print("CHEESE-CAKE WITH ... _FROSTING_*99".isupper())
print("aB".isupper())
print("".islower())
print("cheese-cake with ... _frosting_*99".islower())
print("aB".islower())
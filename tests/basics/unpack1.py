# unpack sequences

a, = 1,     ; print(a)
a, b = 2, 3 ; print(a, b)
a, b, c = 1, 2, 3; print(a, b, c)

a, = range(1); print(a)
a, b = range(2); print(a, b)
a, b, c = range(3); print(a, b, c)

(a) = range(1); print(a)
(a,) = range(1); print(a)
(a, b) = range(2); print(a, b)
(a, b, c) = range(3); print(a, b, c)

# lists

[] = []
[a] = range(1); print(a)
[a, b] = range(2); print(a, b)
[a, b, c] = range(3); print(a, b, c)

# with star

*a, = ()    ; print(a)
*a, = 4,    ; print(a)
*a, = 5, 6  ; print(a)

*a, b = 7,          ; print(a, b)
*a, b = 8, 9        ; print(a, b)
*a, b = 10, 11, 12  ; print(a, b)

a, *b = 13,         ; print(a, b)
a, *b = 14, 15      ; print(a, b)
a, *b = 16, 17, 18  ; print(a, b)

a, *b, c = 19, 20           ; print(a, b)
a, *b, c = 21, 22, 23       ; print(a, b)
a, *b, c = 24, 25, 26, 27   ; print(a, b)

a = [28, 29]
*b, = a
print(a, b, a == b)

[*a] = [1, 2, 3]
print(a)

try:
    a, *b, c = (30,)
except ValueError:
    print("ValueError")

# with star and generic iterator

*a, = range(5)          ; print(a)
*a, b = range(5)        ; print(a, b)
*a, b, c = range(5)     ; print(a, b, c)
a, *b = range(5)        ; print(a, b)
a, *b, c = range(5)     ; print(a, b, c)
a, *b, c, d = range(5)  ; print(a, b, c, d)
a, b, *c = range(5)     ; print(a, b, c)
a, b, *c, d = range(5)  ; print(a, b, c, d)
a, b, *c, d, e = range(5)   ; print(a, b, c, d, e)

*a, = [x * 2 for x in [1, 2, 3, 4]]     ; print(a)
*a, b = [x * 2 for x in [1, 2, 3, 4]]   ; print(a, b)
a, *b = [x * 2 for x in [1, 2, 3, 4]]   ; print(a, b)
a, *b, c = [x * 2 for x in [1, 2, 3, 4]]; print(a, b, c)

try:
    a, *b, c = range(0)
except ValueError:
    print("ValueError")

try:
    a, *b, c = range(1)
except ValueError:
    print("ValueError")

*a, = b
a, *b = c
a, *b, = c
a, *b, c = d

[*a] = b
[*a,] = b
[a, *b] = c
[a, *b,] = c
[a, *b, c] = d

(*a,) = x
(*a, b) = x
(a, *b) = x
(*a, b, c) = x
(a, *b, c) = x
(a, b, *c) = x
(*a, b, c, d) = x
(a, *b, c, d) = x
(a, b, *c, d) = x
(a, b, c, *d) = x

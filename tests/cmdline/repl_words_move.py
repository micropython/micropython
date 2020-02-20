# word movement
# backward-word, start in word
234b1
# backward-word, don't start in word
234    b1
# backward-word on start of line. if cursor is moved, this will result in a SyntaxError
1 2 + 3b+
# forward-word, start in word
1+2 12+f+3
# forward-word, don't start in word
1+    12 3f+
# forward-word on eol. if cursor is moved, this will result in a SyntaxError
1 + 2 3f+

# kill word
# backward-kill-word, start in word
100 + 45623
# backward-kill-word, don't start in word
100 + 456231
# forward-kill-word, start in word
100 + 256d3
# forward-kill-word, don't start in word
1 + 256d2

# extra move/kill shortcuts
# ctrl-left
234[1;5D1
# ctrl-right
12[1;5C3
# ctrl-w
1231

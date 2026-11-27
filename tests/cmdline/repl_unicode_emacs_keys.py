# REPL tests of UTF-8 editing with emacs keys and word movement
# ctrl-B, ctrl-F and ctrl-D move over and delete whole characters
'あう'い
'あいう'x
# ctrl-K and ctrl-U
'あいう''
xyz日本'あい'
# ctrl-P and ctrl-N
'日'
'本'

# word movement treats non-ASCII characters as separators
'ab日本'bx
'日本ab'fc
'日本ab'dx
'ab日本'[1;5Cx
'日本'+'ab'[1;5Dx
'あ' + 'abc'い'
'あ' + 'abc'い'

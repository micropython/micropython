# REPL tests of invalid UTF-8 input: bytes that do not form a character are dropped
# continuation bytes without a lead byte
'a{\x80}{\xbf}b'
# bytes that never start a character, and their continuation bytes
'a{\xf8}{\x88}{\x80}{\x80}{\xff}{\xbf}b'
# a character cut short by ASCII or by another character
'a{\xe3}{\x81}b{\xf0}{\x9f}あ'
# a character cut short by an escape sequence or by Enter
'あ{\xe3}[D{\xe3}x[F'
'い'{\xe3}{\x81}
# a character cut short by Ctrl-C, which cancels the line
'う{\xe3}{\x03}
# input() only returns well-formed characters
input()
え{\xe3}{\x81}
input()
お{\xf8}{\x80}{\x80}{\x80}か

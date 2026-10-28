# REPL tests of UTF-8 line editing
# typed characters are kept
'あいう', len('日本語')
# backspace and delete remove whole characters
'あいう'[D[Dx
'éàü'[D[D[D[3~[Cy
# cursor movement over characters
語'[H'日本
# history recall then edit
'𝄞♪'
[A♫'

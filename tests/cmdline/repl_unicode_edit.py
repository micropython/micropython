# REPL tests of UTF-8 line editing
# typed characters are kept
'あいう', len('日本語')
'é', '𝄞😀', len('é𝄞😀')
# backspace and delete remove whole characters
'あいう'[D[Dx
'éàü'[D[D[D[3~[Cy
'𝄞😀♪'[D
# cursor movement over characters
語'[H'日本
'本'[1~[C日[4~[D語
'ac'[D[Db日
'語'OH[C日本OF
# history recall then edit
'𝄞♪'
[A♫'
# a recalled line with fewer characters but as many bytes erases the old one
'😀😀'
'abcdefgh'[A
'ab'[A[A[B[B'あ'
# input() edits the same way
input()
あいう[D[D[3~x
# tab completion after UTF-8 characters
s = 'あい'
'日本', s.sta{\x09}('あ')
# a long line, then recall and edit it
len('あいうえおabcdeあいうえおabcdeあいうえおabcdeあいうえおabcdeあいうえおabcdeあいうえおabcdeあいうえおabcdeあいうえおabcde')[H[C[C[C[C[C[3~[3~
[A[H[C[C[C[C[Cx
# editing stays within a continuation line
if 1:
print('あいう')[D[D[D[H[F




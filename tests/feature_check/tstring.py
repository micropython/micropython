# check whether t-strings (PEP-750) are supported

# TODO remove this check when micropython-lib's string extends ustring
from string.templatelib import Template, Interpolation

a = 1
t = t"a={a}"
print("tstring")

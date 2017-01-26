# test printing debugging info when compiling
import ure
ure.compile('^a|b[0-9]\w$', ure.DEBUG)

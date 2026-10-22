# check if set and frozenset are enabled
import builtins

print(hasattr(builtins, "set"), hasattr(builtins, "frozenset"))

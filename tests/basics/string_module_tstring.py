# Test basic templatelib functionality.
# This test requires t-strings support.

from string.templatelib import Template

print("templatelib", isinstance(t"hi", Template))

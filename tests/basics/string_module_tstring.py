# Test requires t-strings support
# SPDX-License-Identifier: MIT

from string.templatelib import Template

# Test templatelib functionality
print("templatelib", isinstance(t"hi", Template))

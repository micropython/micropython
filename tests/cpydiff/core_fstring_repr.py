"""
categories: Core,f-strings
description: f-strings don't support !a conversions
cause: MicropPython does not implement ascii()
workaround: None
"""

f"{'unicode text'!a}"

"""
categories: Core
description: f-strings don't support !a conversions
cause: MicropPython does not implement ascii()
workaround: Unknown
"""

f"{'unicode text'!a}"
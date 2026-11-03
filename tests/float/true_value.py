# Test true-ish value handling
# requires: complex float

if not 0.0:
    print("float 0")

if not 0 + 0j:
    print("complex 0")

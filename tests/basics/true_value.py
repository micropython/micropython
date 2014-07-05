# Test true-ish value handling

if not False:
    print("False")

if not None:
    print("None")

if not 0:
    print("0")

if not "":
    print("Empty string")
if "foo":
    print("Non-empty string")

if not ():
    print("Empty tuple")
if ("",):
    print("Non-empty tuple")

if not []:
    print("Empty list")
if [0]:
    print("Non-empty list")

if not {}:
    print("Empty dict")
if {0:0}:
    print("Non-empty dict")

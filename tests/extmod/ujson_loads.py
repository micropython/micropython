try:
    import ujson as json
except ImportError:
    try:
        import json
    except ImportError:
        print("SKIP")
        raise SystemExit

try:
    extra_coverage
    is_coverage = True
except NameError:
    is_coverage = False


def my_print(o):
    if isinstance(o, dict):
        print("sorted dict", sorted(o.items()))
    else:
        print(o)


my_print(json.loads("null"))
my_print(json.loads("false"))
my_print(json.loads("true"))
my_print(json.loads("1"))
my_print(json.loads("-2"))
my_print(json.loads('"abc\\u0064e"'))
my_print(json.loads("[]"))
my_print(json.loads("[null]"))
my_print(json.loads("[null,false,true]"))
my_print(json.loads(" [ null , false , true ] "))
my_print(json.loads("{}"))
my_print(json.loads('{"a":true}'))
my_print(json.loads('{"a":null, "b":false, "c":true}'))
my_print(json.loads('{"a":[], "b":[1], "c":{"3":4}}'))
my_print(json.loads('"abc\\bdef"'))
my_print(json.loads('"abc\\fdef"'))
my_print(json.loads('"abc\\ndef"'))
my_print(json.loads('"abc\\rdef"'))
my_print(json.loads('"abc\\tdef"'))
my_print(json.loads('"abc\\uabcd"'))

if is_coverage:  # Has ordered json loads enabled
    o = json.loads('{"b":null, "c":false, "e":true}')
    print(list(o.items()) == list(sorted(o.items())))
else:
    print(True)

# whitespace handling
my_print(json.loads('{\n\t"a":[]\r\n, "b":[1], "c":{"3":4}     \n\r\t\r\r\r\n}'))

# loading nothing should raise exception
try:
    json.loads("")
except ValueError:
    print("ValueError")

# string which is not closed
try:
    my_print(json.loads('"abc'))
except ValueError:
    print("ValueError")

# unaccompanied closing brace
try:
    my_print(json.loads("]"))
except ValueError:
    print("ValueError")

# unspecified object type
try:
    my_print(json.loads("a"))
except ValueError:
    print("ValueError")

# bad property name
try:
    my_print(json.loads('{{}:"abc"}'))
except ValueError:
    print("ValueError")

# unexpected characters after white space
try:
    my_print(json.loads("[null]   a"))
except ValueError:
    print("ValueError")

try:
    import ujson as json
except:
    import json

def my_print(o):
    if isinstance(o, dict):
        print('sorted dict', sorted(o.items()))
    else:
        print(o)

my_print(json.loads('null'))
my_print(json.loads('false'))
my_print(json.loads('true'))
my_print(json.loads('1'))
my_print(json.loads('1.2'))
my_print(json.loads('1e2'))
my_print(json.loads('-2'))
my_print(json.loads('-2.3'))
my_print(json.loads('-2e3'))
my_print(json.loads('-2e-3'))
my_print(json.loads('"abc\\u0064e"'))
my_print(json.loads('[]'))
my_print(json.loads('[null]'))
my_print(json.loads('[null,false,true]'))
my_print(json.loads(' [ null , false , true ] '))
my_print(json.loads('{}'))
my_print(json.loads('{"a":true}'))
my_print(json.loads('{"a":null, "b":false, "c":true}'))
my_print(json.loads('{"a":[], "b":[1], "c":{"3":4}}'))

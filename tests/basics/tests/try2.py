# nested try's

try:
    print("try 1")
    try:
        print("try 2")
        foo()
    except:
        print("except 2")
    bar()
except:
    print("except 1")

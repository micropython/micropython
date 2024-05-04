

def Log(*messages):

    if type(messages[0]) is int:
        if messages[0] < 4 :
            print(messages)
    else :
        print(messages)
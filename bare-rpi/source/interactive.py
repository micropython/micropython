import cio
#import sys

def getchar ():
#    return ord(sys.stdin.read(1));
    return cio.getchar();

s=""
cio.write ("Hello World!")
while (1):
    rc = getchar()
    print ( rc );
    print ( str(chr( rc )) )
    if ( rc == 0x08 ):      # backspace
        if (len(s) >= 1):
            s = s[:-1]
            pass
        else:
            s = ""
    elif ( rc == '\n' or rc=='\r' or rc=='\\' ):    # newline -> try compiling and evaluating
        try:
            print (eval ( compile ( s, "repl", "exec")));
            s = "";
        except:
            print ("MÖP");
            s = s + chr( rc );
    elif ( rc == 'b' ) :    # b as debug-character
        print ( s );
    else:
        s = s + chr( rc )
    print (s)
print ("UPS")

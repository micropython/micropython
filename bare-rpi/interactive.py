#import cio
import sys

def getchar ():
    return ord(sys.stdin.read(1));

s=""
while (1):
    rc = getchar()
    #print ( rc );
    print ( str(chr( rc )), end="" )
    if ( rc == 0x08 ):      # backspace
        if (len(s) >= 1):
            s = s[0:(len(s)-1)]
            pass
        else:
            s = ""
    elif ( rc == '\n' or rc=='\r' ):    # newline -> try compiling and evaluating
        try:
            print (eval ( compile ( s, "repl", "exec")));
            s = "";
        except:
            s = s + chr( rc );
    elif ( rc == 'b' ) :    # b as debug-character
        print ( s );
    else:
        s = s + chr( rc )

print ("UPS")


# expose all functions from C module on this Python module
from micropython_module import *
import sys

def main():
    return run_main(sys.argv)

if __name__ == '__main__':
    main()

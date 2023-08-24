import sys
from . import assemble_file


def main(fn):
    assemble_file(fn)


if __name__ == '__main__':
    main(sys.argv[1])


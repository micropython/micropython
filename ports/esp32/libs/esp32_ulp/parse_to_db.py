import sys

from .preprocess import Preprocessor
from .definesdb import DefinesDB


def parse(files):
    db = DefinesDB()

    p = Preprocessor()
    p.use_db(db)

    for f in files:
        print('Processing file:', f)

        p.process_include_file(f)

    print('Done.')


if __name__ == '__main__':
    parse(sys.argv[1:])


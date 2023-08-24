import os
# import btree
from .util import file_exists

DBNAME = 'defines.db'


class DefinesDB:
    def __init__(self):
        self._file = None
        self._db = None
        self._db_exists = None

    def clear(self):
        self.close()
        try:
            os.remove(DBNAME)
            self._db_exists = False
        except OSError:
            pass

    def is_open(self):
        return self._db is not None

    def open(self):
        if self.is_open():
            return
        try:
            self._file = open(DBNAME, 'r+b')
        except OSError:
            self._file = open(DBNAME, 'w+b')
        # self._db = btree.open(self._file)
        self._db_exists = True

    def close(self):
        if not self.is_open():
            return
        self._db.close()
        self._db = None
        self._file.close()
        self._file = None

    def db_exists(self):
        if self._db_exists is None:
            self._db_exists = file_exists(DBNAME)
        return self._db_exists

    def update(self, dictionary):
        for k, v in dictionary.items():
            self.__setitem__(k, v)

    def get(self, key, default):
        try:
            result = self.__getitem__(key)
        except KeyError:
            result = default
        return result

    def keys(self):
        if not self.db_exists():
            return []

        self.open()
        return [k.decode() for k in self._db.keys()]

    def __getitem__(self, key):
        if not self.db_exists():
            raise KeyError

        self.open()
        return self._db[key.encode()].decode()

    def __setitem__(self, key, value):
        self.open()
        self._db[key.encode()] = str(value).encode()

    def __iter__(self):
        return iter(self.keys())

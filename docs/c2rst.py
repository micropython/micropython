import sphinx.parsers
import docutils.parsers.rst as rst

class CStrip(sphinx.parsers.Parser):
  def __init(self):
    self.rst_parser = rst.Parser()

  def parse(self, inputstring, document):
    print(inputstring)
    self.rst_parser(stripped, document)

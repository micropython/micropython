import docutils.parsers
import docutils.parsers.rst as rst

class CStrip(docutils.parsers.Parser):
  def __init__(self):
    self.rst_parser = rst.Parser()

  def parse(self, inputstring, document):
    stripped = []
    for line in inputstring.split("\n"):
      line = line.strip()
      if line == "//|":
        stripped.append("")
      elif line.startswith("//| "):
        stripped.append(line[len("//| "):])
    stripped = "\r\n".join(stripped)
    self.rst_parser.parse(stripped, document)

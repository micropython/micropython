import sphinx.parsers

class CStrip(sphinx.parsers.Parser):
    def __init__(self):
        self.rst_parser = sphinx.parsers.RSTParser()

    def parse(self, inputstring, document):
        # This setting is missing starting with Sphinx 1.7.1 so we set it ourself.
        document.settings.tab_width = 4
        stripped = []
        for line in inputstring.split("\n"):
            line = line.strip()
            if line == "//|":
                stripped.append("")
            elif line.startswith("//| "):
                stripped.append(line[len("//| "):])
        stripped = "\r\n".join(stripped)
        self.rst_parser.parse(stripped, document)

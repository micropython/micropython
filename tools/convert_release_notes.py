import sys
import mistune

print(sys.argv[1])

with open(sys.argv[1], "r") as source_file:
    source = source_file.read()

html = mistune.Markdown()

print()
print("HTML")
print("=====================================")
print("From the <a href=\"\">GitHub release page</a>:\n<blockquote>")
print(html(source))
print("</blockquote>")

class AdafruitBBCodeRenderer:
    def __init__(self, **kwargs):
        self.options = kwargs

    def placeholder(self):
        return ''

    def paragraph(self, text):
        return text + "\n\n"

    def text(self, text):
        return text

    def link(self, link, title, text):
        return "[url={}]{}[/url]".format(link, text)

    def autolink(self, link, is_email):
        if not is_email:
            return "[url={}]{}[/url]".format(link, link)
        return link

    def header(self, text, level, raw):
        return "[b][size=150]{}[/size][/b]\n".format(text)

    def codespan(self, text):
        return "[color=#E74C3C][size=95]{}[/size][/color]".format(text)

    def list_item(self, text):
        return "[*]{}[/*]\n".format(text.strip())

    def list(self, body, ordered=True):
        ordered_indicator = "=" if ordered else ""
        return "[list{}]\n{}[/list]".format(ordered_indicator, body)

    def double_emphasis(self, text):
        return "[b]{}[/b]".format(text)

    def emphasis(self, text):
        return "[b]{}[/b]".format(text)

bbcode = mistune.Markdown(renderer=AdafruitBBCodeRenderer())

print()
print("BBCode")
print("=====================================")
print("From the [url=]GitHub release page[/url]:\n[quote]")
print(bbcode(source))
print("[/quote]")

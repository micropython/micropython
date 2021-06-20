# SPDX-FileCopyrightText: 2014 MicroPython & CircuitPython contributors (https://github.com/adafruit/circuitpython/graphs/contributors)
#
# SPDX-License-Identifier: MIT

import sys
import mistune
import mistune.renderers

print(sys.argv[1])

with open(sys.argv[1], "r") as source_file:
    source = source_file.read()

html = mistune.create_markdown()

print()
print("HTML")
print("=====================================")
print('From the <a href="">GitHub release page</a>:\n<blockquote>')
print(html(source))
print("</blockquote>")


class AdafruitBBCodeRenderer(mistune.renderers.BaseRenderer):
    def placeholder(self):
        return ""

    def paragraph(self, text):
        return text + "\n\n"

    def block_text(self, text):
        return text

    def text(self, text):
        return text

    def link(self, link, title, text):
        return "[url={}]{}[/url]".format(link, title)

    def autolink(self, link, is_email):
        if not is_email:
            return "[url={}]{}[/url]".format(link, link)
        return link

    def heading(self, text, level):
        return "[b][size=150]{}[/size][/b]\n".format(text)

    def codespan(self, text):
        return "[color=#E74C3C][size=95]{}[/size][/color]".format(text)

    def list_item(self, text, level):
        return "[*]{}[/*]\n".format(text.strip())

    def list(self, text, ordered, level, start=None):
        ordered_indicator = "=" if ordered else ""
        return "[list{}]\n{}[/list]".format(ordered_indicator, text)

    def double_emphasis(self, text):
        return "[b]{}[/b]".format(text)

    def emphasis(self, text):
        return "[b]{}[/b]".format(text)

    def strong(self, text):
        return "[i]{}[/i]".format(text)


bbcode = mistune.create_markdown(renderer=AdafruitBBCodeRenderer())

print()
print("BBCode")
print("=====================================")
print("From the [url=]GitHub release page[/url]:\n[quote]")
print(bbcode(source))
print("[/quote]")

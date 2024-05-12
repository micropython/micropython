# SPDX-FileCopyrightText: 2014 MicroPython & CircuitPython contributors (https://github.com/adafruit/circuitpython/graphs/contributors)
#
# SPDX-License-Identifier: MIT

import sys
import mistune
from mistune import BlockState

from typing import Dict, Any

print(sys.argv[1])

with open(sys.argv[1], "r") as source_file:
    source = source_file.read()

html = mistune.create_markdown()

print()
print("HTML")
print("=====================================")
print('<p><em>From the <a href="">GitHub release page</a>:</em></p>\n')
print(html(source))


class AdafruitBBCodeRenderer(mistune.BaseRenderer):
    def render_token(self, token: Dict[str, Any], state: BlockState) -> str:
        # backward compatible with v2
        func = self._get_method(token["type"])
        attrs = token.get("attrs")

        if "raw" in token:
            text = token["raw"]
        elif "children" in token:
            text = self.render_tokens(token["children"], state)
        else:
            if attrs:
                return func(**attrs)
            else:
                return func()
        if attrs:
            return func(text, **attrs)
        else:
            return func(text)

    def blank_line(self):
        return ""

    def paragraph(self, text):
        return f"{text}\n\n"

    def block_text(self, text):
        return text

    def text(self, text):
        return text

    def block_code(self, text):
        return f"[code]{text}[/code]"

    def link(self, text, url):
        return f"[url={url}]{text}[/url]"

    def autolink(self, link, is_email):
        if not is_email:
            return f"[url={link}]{link}[/url]"
        return link

    def heading(self, text, level):
        return f"[b][size={200 - level * 20}]{text}[/size][/b]\n"

    def codespan(self, text):
        return f"[color=#E74C3C][size=95]{text}[/size][/color]"

    def list(self, text, ordered: bool, start=None, depth=None):
        ordered_indicator = "=" if ordered else ""
        return f"[list{ordered_indicator}]\n{text}[/list]"

    def list_item(self, text):
        return f"[*]{text.strip()}[/*]\n"

    def double_emphasis(self, text):
        return f"[b]{text}[/b]"

    def emphasis(self, text):
        return f"[i]{text}[/i]"

    def strong(self, text):
        return f"[b]{text}[/b]"

    def finalize(self, data):
        return "".join(data)


bbcode = mistune.create_markdown(renderer=AdafruitBBCodeRenderer())

print()
print("BBCode")
print("=====================================")
print("[i]From the [url=]GitHub release page[/url]:[/i]\n")
print(bbcode(source))

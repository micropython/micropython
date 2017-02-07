#
# This is a Sphinx documentation tool extension which makes .only::
# directives be eagerly processed early in the parsing stage. This
# makes sure that content in .only:: blocks gets actually excluded
# as a typical user expects, instead of bits of information in
# these blocks leaking to documentation in various ways (e.g.,
# indexes containing entries for functions which are actually in
# .only:: blocks and thus excluded from documentation, etc.)
# Note that with this extension, you may need to completely
# rebuild a doctree when switching builders (i.e. completely
# remove _build/doctree dir between generation of HTML vs PDF
# documentation).
#
# This extension works by monkey-patching Sphinx core, so potentially
# may not work with untested Sphinx versions. It tested to work with
# 1.2.2 and 1.4.2
#
# Copyright (c) 2016 Paul Sokolovsky
# Based on idea by Andrea Cassioli:
# https://github.com/sphinx-doc/sphinx/issues/2150#issuecomment-171912290
# Licensed under the terms of BSD license, see LICENSE file.
#
import sphinx
from docutils.parsers.rst import directives


class EagerOnly(sphinx.directives.other.Only):

    def run(self, *args):
        # Evaluate the condition eagerly, and if false return no nodes right away
        env = self.state.document.settings.env
        env.app.builder.tags.add('TRUE')
        #print(repr(self.arguments[0]))
        if not env.app.builder.tags.eval_condition(self.arguments[0]):
            return []

        # Otherwise, do the usual processing
        nodes = super(EagerOnly, self).run()
        if len(nodes) == 1:
            nodes[0]['expr'] = 'TRUE'
        return nodes


def setup(app):
    directives.register_directive('only', EagerOnly)

#
# This is a Sphinx documentation tool extension which allows to
# automatically exclude from full-text search index document
# which are not referenced via toctree::. It's intended to be
# used with toctrees conditional on only:: directive, with the
# idea being that if you didn't include it in the ToC, you don't
# want the docs being findable by search either (for example,
# because these docs contain information not pertinent to a
# particular product configuration).
#
# This extension depends on "eager_only" extension and won't work
# without it.
#
# Copyright (c) 2016 Paul Sokolovsky
# Licensed under the terms of BSD license, see LICENSE file.
#
import sphinx


org_StandaloneHTMLBuilder_index_page = None


def StandaloneHTMLBuilder_index_page(self, pagename, doctree, title):
    if pagename not in self.env.files_to_rebuild:
        if pagename != self.env.config.master_doc and 'orphan' not in self.env.metadata[pagename]:
            print("Excluding %s from full-text index because it's not referenced in ToC" % pagename)
            return
    return org_StandaloneHTMLBuilder_index_page(self, pagename, doctree, title)


def setup(app):
    global org_StandaloneHTMLBuilder_index_page
    org_StandaloneHTMLBuilder_index_page = sphinx.builders.html.StandaloneHTMLBuilder.index_page
    sphinx.builders.html.StandaloneHTMLBuilder.index_page = StandaloneHTMLBuilder_index_page

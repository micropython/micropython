# Derived from code on Eric Holscher's blog, found at:
# https://www.ericholscher.com/blog/2016/jul/25/integrating-jinja-rst-sphinx/

import re

def render_with_jinja(docname, source):
    if re.search('^\s*.. jinja$', source[0], re.M):
        return True
    return False

def rstjinja(app, docname, source):
    """
    Render our pages as a jinja template for fancy templating goodness.
    """
    # Make sure we're outputting HTML
    if app.builder.format not in ("html", "latex"):
        return

    # we only want specific files to run through this func
    if not render_with_jinja(docname, source):
        return

    src = rendered = source[0]
    print(f"rendering {docname} as jinja templates")

    if app.builder.format == "html":
        rendered = app.builder.templates.render_string(
            src, app.config.html_context
        )
    else:
        from sphinx.util.template import BaseRenderer
        renderer = BaseRenderer()
        rendered = renderer.render_string(
            src,
            app.config.html_context
        )

    source[0] = rendered

def setup(app):
    app.connect("source-read", rstjinja)

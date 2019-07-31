def c2rst(app, docname, source):
    """ Pre-parse '.c' & '.h' files that contain rST source.
    """
    # Make sure we're outputting HTML
    if app.builder.format != 'html':
        return

    fname = app.env.doc2path(docname)
    if (not fname.endswith(".c") and
        not fname.endswith(".h")):
            #print("skipping:", fname)
            return

    src = source[0]

    stripped = []
    for line in src.split("\n"):
        line = line.strip()
        if line == "//|":
            stripped.append("")
        elif line.startswith("//| "):
            stripped.append(line[len("//| "):])
    stripped = "\r\n".join(stripped)

    rendered = app.builder.templates.render_string(
        stripped, app.config.html_context
    )
    source[0] = rendered

def setup(app):
    app.connect("source-read", c2rst)

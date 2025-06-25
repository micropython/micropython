def edit(filename=None):
    if filename is None:
        from . import picker

        filename = picker.pick_file()
    from . import editor

    editor.edit(filename)

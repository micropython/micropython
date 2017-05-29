#
# This is a Sphinx documentation tool extension which allows to
# exclude some Python modules from the generated indexes. Modules
# are excluded both from "modindex" and "genindex" index tables
# (in the latter case, all members of a module are excluded).
# To control exclusion, set "modindex_exclude" variable in Sphinx
# conf.py to the list of modules to exclude. Note: these should be
# modules (as defined by py:module directive, not just raw filenames).
# This extension works by monkey-patching Sphinx core, so potentially
# may not work with untested Sphinx versions. It tested to work with
# 1.2.2 and 1.4.2
#
# Copyright (c) 2016 Paul Sokolovsky
# Licensed under the terms of BSD license, see LICENSE file.
#
import sphinx


#org_PythonModuleIndex_generate = None
org_PyObject_add_target_and_index = None
org_PyModule_run = None

EXCLUDES = {}

# No longer used, PyModule_run() monkey-patch does all the job
def PythonModuleIndex_generate(self, docnames=None):
    docnames = []
    excludes = self.domain.env.config['modindex_exclude']
    for modname, (docname, synopsis, platforms, deprecated) in self.domain.data['modules'].items():
        #print(docname)
        if modname not in excludes:
            docnames.append(docname)

    return org_PythonModuleIndex_generate(self, docnames)


def PyObject_add_target_and_index(self, name_cls, sig, signode):
    if hasattr(self.env, "ref_context"):
        # Sphinx 1.4
        ref_context = self.env.ref_context
    else:
        # Sphinx 1.2
        ref_context = self.env.temp_data
    modname = self.options.get(
        'module', ref_context.get('py:module'))
    #print("*", modname, name_cls)
    if modname in self.env.config['modindex_exclude']:
        return None
    return org_PyObject_add_target_and_index(self, name_cls, sig, signode)


def PyModule_run(self):
    env = self.state.document.settings.env
    modname = self.arguments[0].strip()
    excl = env.config['modindex_exclude']
    if modname in excl:
        self.options['noindex'] = True
        EXCLUDES.setdefault(modname, []).append(env.docname)
    return org_PyModule_run(self)


def setup(app):
    app.add_config_value('modindex_exclude', [], 'html')

#    global org_PythonModuleIndex_generate
#    org_PythonModuleIndex_generate = sphinx.domains.python.PythonModuleIndex.generate
#    sphinx.domains.python.PythonModuleIndex.generate = PythonModuleIndex_generate

    global org_PyObject_add_target_and_index
    org_PyObject_add_target_and_index = sphinx.domains.python.PyObject.add_target_and_index
    sphinx.domains.python.PyObject.add_target_and_index = PyObject_add_target_and_index

    global org_PyModule_run
    org_PyModule_run = sphinx.domains.python.PyModule.run
    sphinx.domains.python.PyModule.run = PyModule_run

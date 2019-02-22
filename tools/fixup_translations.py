# Validate that all entries in the .pot are in every .po. Only the .pot is updated so we can detect
# if a translation was added to the source but isn't in a .po. This ensures translators can grab
# complete files to work on.

import git
import sys
import polib

po_filenames = sys.argv[1:]
repo = git.Repo()

NO_TRANSLATION_WHITELIST = ["ffi_prep_closure_loc"]

bad_commits = {}
for po_filename in po_filenames:
    print("Checking", po_filename)

    commits = repo.iter_commits(paths=po_filename, reverse=True, topo_order=True)
    first_translations = None
    fixed_ids = set()
    for commit in commits:
        try:
            blob = commit.tree / po_filename
        except KeyError:
            continue
        try:
            current_file = polib.pofile(blob.data_stream.read().decode("utf-8"))
        except OSError:
            print("skipping invalid po in", commit)
            continue
        if not first_translations:
            first_translations = current_file
            continue
        print(commit.authored_date, commit)
        first_translations.metadata = current_file.metadata
        for entry in first_translations:
            if entry.msgid == "soft reboot\n":
                continue
            newer_entry = current_file.find(entry.msgid)
            if newer_entry and entry.msgstr != newer_entry.msgstr:
                if newer_entry.msgstr != "" and (newer_entry.msgstr != entry.msgid or entry.msgid in NO_TRANSLATION_WHITELIST):
                    entry.merge(newer_entry)
                    entry.msgstr = newer_entry.msgstr
                elif entry.msgid not in fixed_ids:
                    if commit not in bad_commits:
                        bad_commits[commit] = set()
                    bad_commits[commit].add(po_filename)
                    fixed_ids.add(entry.msgid)
                    print(entry.msgid, "\"" + entry.msgstr + "\"", "\"" + newer_entry.msgstr + "\"")

    first_translations.save(po_filename)

print()
for commit in bad_commits:
    files = bad_commits[commit]
    print(commit)
    for file in files:
        print("\t",file)

# SPDX-FileCopyrightText: 2014 MicroPython & CircuitPython contributors (https://github.com/adafruit/circuitpython/graphs/contributors)
#
# SPDX-License-Identifier: MIT

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
        first_translations.header = current_file.header
        for entry in first_translations:
            newer_entry = current_file.find(entry.msgid)

            if newer_entry and entry.msgid == "":
                entry.merge(newer_entry)
                print(entry)
            elif newer_entry and entry.msgstr != newer_entry.msgstr:
                if newer_entry.msgstr != "" and (
                    newer_entry.msgstr != entry.msgid or entry.msgid in NO_TRANSLATION_WHITELIST
                ):
                    entry.merge(newer_entry)
                    entry.msgstr = newer_entry.msgstr
                    if "fuzzy" not in newer_entry.flags and "fuzzy" in entry.flags:
                        entry.flags.remove("fuzzy")
                elif entry.msgid not in fixed_ids:
                    if commit not in bad_commits:
                        bad_commits[commit] = set()
                    bad_commits[commit].add(po_filename)
                    fixed_ids.add(entry.msgid)
                    # print(entry.msgid, "\"" + entry.msgstr + "\"", "\"" + newer_entry.msgstr + "\"",)
            elif newer_entry and newer_entry.flags != entry.flags:
                entry.flags = newer_entry.flags
            elif newer_entry and newer_entry.obsolete != entry.obsolete:
                entry.obsolete = newer_entry.obsolete
            elif not newer_entry:
                entry.obsolete = True

        # Add new entries to the modified pofile.
        for entry in current_file:
            old_entry = first_translations.find(entry.msgid, include_obsolete_entries=True)
            if old_entry:
                continue
            first_translations.append(entry)

    # Remove obsolete translations. We can always use this script to revive them from the git history.
    to_remove = []
    for entry in first_translations:
        if entry.obsolete:
            to_remove.append(entry)

    for remove in to_remove:
        first_translations.remove(remove)

    first_translations.save(po_filename)

print()
for commit in bad_commits:
    files = bad_commits[commit]
    print(commit)
    for file in files:
        print("\t", file)

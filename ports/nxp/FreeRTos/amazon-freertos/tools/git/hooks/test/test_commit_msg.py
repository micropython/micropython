#!/usr/bin/python

import os
import sys
myPath = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, os.path.dirname(myPath))

import pytest

import src.commit_msg as commit_msg


allowed_labels = [
    ('chore'),
    ('Chore'),
    ('docs'),
    ('feat'),
    ('fix'),
    ('merge'),
    ('refactor'),
    ('revert'),
    ('style'),
    ('test'),
    ('tool'),
]
@pytest.mark.parametrize("allowed_label", allowed_labels)
def test_LabelIsValid_returns_true_when_label_is_valid(allowed_label):
    assert commit_msg.LabelIsValid(allowed_label)


invalid_labels = [
    ('foo'),
    ('bar'),
    ('baz'),
    ('feature'),
    ('this'),
    ('that'),
]
@pytest.mark.parametrize("invalid_label", invalid_labels)
def test_LabelIsValid_returns_false_when_label_is_not_valid(invalid_label):
    assert False == commit_msg.LabelIsValid(invalid_label)


subject_label_pairs = [
    ('chore: This is a valid subject', 'chore'),
    ('docs: This is a valid subject', 'docs'),
    ('feat: This is a valid subject', 'feat'),
    ('bar: This is a valid subject', 'bar'),
    ('baz: This is a valid subject', 'baz'),
    ('feature: This is a valid subject', 'feature'),
]
@pytest.mark.parametrize("subject, expected_label", subject_label_pairs)
def test_LabelFromSubject_returns_label_in_subject(subject, expected_label):
    actual_label = commit_msg.LabelFromSubject(subject)
    assert expected_label == actual_label


subjects = [
    ("this: Is a valid subject", True),
    ("this: Subject is the longest allowed text text text text tex", True),
    ("this: Subject is too long text text text text text text textt", False),
    ("This subject has no label", False),
    ("", False),
    ("Merge pull", True),
    ("Merge branch", True),
]
@pytest.mark.parametrize("subject, expected", subjects)
def test_SubjectIsValid(subject, expected):
    assert expected == commit_msg.SubjectIsValid(subject)


long_text_lines = [
    ('text text text text text text text text text text text text text text text textt', True),
    ('text text text text text text text text text text text text text text text texttt', False),
    ('text text text text text text text text text text text text text text text textt ', True),
    ('text text text text text text text text text text text text text text text textt\n', True),
    ('text text text text text text text text text text text text text text text textt#Not a comment', False),
    ('#Long comments do not count against the line limit text text text text text text text text ', True),
]
@pytest.mark.parametrize("line_text, expected", long_text_lines)
def test_LineLengthIsValid(line_text, expected):
    assert expected == commit_msg.LineLengthIsValid(line_text)


blank_lines = [
    ("",True),
    ("\n",True),
    ("#comments don't count",True),
    (" #but this is not a comment",False),
    ("i",False),
]
@pytest.mark.parametrize("blank_line, expected", blank_lines)
def test_LineIsBlank(blank_line, expected):
    assert expected == commit_msg.LineIsBlank(blank_line)

commit_files = [
    ("test/valid_commit.md", True),
    ("test/long_subject.md", False),
    ("test/no_sep.md", False),
    ("test/long_body_line.md", False),
    ("test/bad_label.md", False),
    ("test/merge_commit.md", True),
]
@pytest.mark.parametrize("commit_file, expected", commit_files)
def test_CommitFileIsValid(commit_file, expected):
    assert expected == commit_msg.CommitFileIsValid(commit_file)

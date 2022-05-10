# SPDX-FileCopyrightText: 2017 Yoch <https://github.com/yoch>
#
# SPDX-License-Identifier: EPL-1.0

"""
`matcher`
====================================================================================

MQTT topic filter matcher from the Eclipse Project's Paho.MQTT.Python
https://github.com/eclipse/paho.mqtt.python/blob/master/src/paho/mqtt/matcher.py
* Author(s): Yoch (https://github.com/yoch)
"""


class MQTTMatcher:
    """Intended to manage topic filters including wildcards.

    Internally, MQTTMatcher use a prefix tree (trie) to store
    values associated with filters, and has an iter_match()
    method to iterate efficiently over all filters that match
    some topic name.
    """

    # pylint: disable=too-few-public-methods
    class Node:
        """Individual node on the MQTT prefix tree."""

        __slots__ = "children", "content"

        def __init__(self):
            self.children = {}
            self.content = None

    def __init__(self):
        self._root = self.Node()

    def __setitem__(self, key, value):
        """Add a topic filter :key to the prefix tree
        and associate it to :value"""
        node = self._root
        for sym in key.split("/"):
            node = node.children.setdefault(sym, self.Node())
        node.content = value

    def __getitem__(self, key):
        """Retrieve the value associated with some topic filter :key"""
        try:
            node = self._root
            for sym in key.split("/"):
                node = node.children[sym]
            if node.content is None:
                raise KeyError(key)
            return node.content
        except KeyError:
            raise KeyError(key) from None

    def __delitem__(self, key):
        """Delete the value associated with some topic filter :key"""
        lst = []
        try:
            parent, node = None, self._root
            for k in key.split("/"):
                parent, node = node, node.children[k]
                lst.append((parent, k, node))
            node.content = None
        except KeyError:
            raise KeyError(key) from None
        else:  # cleanup
            for parent, k, node in reversed(lst):
                if node.children or node.content is not None:
                    break
                del parent.children[k]

    def iter_match(self, topic):
        """Return an iterator on all values associated with filters
        that match the :topic"""
        lst = topic.split("/")
        normal = not topic.startswith("$")

        def rec(node, i=0):
            if i == len(lst):
                if node.content is not None:
                    yield node.content
            else:
                part = lst[i]
                if part in node.children:
                    for content in rec(node.children[part], i + 1):
                        yield content
                if "+" in node.children and (normal or i > 0):
                    for content in rec(node.children["+"], i + 1):
                        yield content
            if "#" in node.children and (normal or i > 0):
                content = node.children["#"].content
                if content is not None:
                    yield content

        return rec(self._root)

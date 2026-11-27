try:
    from io import StringIO
    import json
except ImportError:
    print("SKIP")
    raise SystemExit


class Node:
    def __init__(self, value, left=None, right=None):
        self.value = value
        self.left = left
        self.right = right


def node_default(obj):
    if isinstance(obj, Node):
        d = {
            "left": obj.left,
            "right": obj.right,
            "value": obj.value,
        }
        return {k: d[k] for k in sorted(d.keys())}
    raise TypeError("Object of type %s is not JSON serializable" % type(obj).__name__)


tree = Node(1, Node(2), Node(3, Node(4), None))
node_list = [Node(i) for i in range(5, 8)]

s = StringIO()
json.dump(tree, s, default=node_default)
print(s.getvalue())

s1 = StringIO()
json.dump({"nodes": node_list}, s1, default=node_default)
print(s1.getvalue())

# Invalid serializing of an object without a default function
try:
    json.dump(Node(10), s, default=None)
except TypeError:
    print("Exception")


class Square:
    def __init__(self, side):
        self.side = side


# Invalid serializing of an object without a proper default function
try:
    json.dump(Square(5), s, default=node_default)
except TypeError:
    print("Exception")


# Invalid default function type
try:
    json.dump(Node(10), s, default=123)
except TypeError:
    print("Exception")

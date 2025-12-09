try:
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

print(json.dumps(tree, default=node_default))
print(json.dumps({"nodes": node_list}, default=node_default))

# Invalid serializing of an object without a default function
try:
    print(json.dumps(Node(10)))
except TypeError:
    print("Exception")


class Square:
    def __init__(self, side):
        self.side = side


# Invalid serializing of an object without a proper default function
try:
    print(json.dumps(Square(5), default=node_default))
except TypeError:
    print("Exception")


# Invalid default function type
try:
    print(json.dumps(Node(10), default=123))
except TypeError:
    print("Exception")

try:
    import json
except ImportError:
    print("SKIP")
    raise SystemExit


def my_print(o):
    if isinstance(o, dict):
        print("sorted dict", sorted(o.items()))
    else:
        print(o)


try:
    # here one closing ] is missing in line with "a"
    json_str_nested = """
    {
        "a" : [["c", ["d"]],
        "b": {
                "bb": 0
        }
    }
    """
    json.loads(json_str_nested)  # <== this should raise an error
except ValueError:
    print("ValueError")

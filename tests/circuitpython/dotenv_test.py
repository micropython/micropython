import dotenv

FILE = __file__.rsplit(".", 1)[0] + ".env"

print(f"notpresent={dotenv.get_key(FILE, 'notpresent')}")
print(f"plain_value={dotenv.get_key(FILE, 'plain_value')}")
print(f"value_with_comment={dotenv.get_key(FILE, 'value_with_comment')}")
print(f"quoted_value={dotenv.get_key(FILE, 'quoted_value')}")
print(f"quoted_value_with_comment={dotenv.get_key(FILE, 'quoted_value_with_comment')}")
print(f"should_be_none={dotenv.get_key(FILE, 'should_be_none')}")
print(f"should_be_empty_string={dotenv.get_key(FILE, 'should_be_empty_string')}")
print(f"should_be_hash={dotenv.get_key(FILE, 'should_be_hash')}")
print(f"quoted_should_be_empty_string={dotenv.get_key(FILE, 'quoted_should_be_empty_string')}")
print(f"duplicate_key={dotenv.get_key(FILE, 'duplicate_key')}")
### This is the a difference from CPython dotenv. The trailing #value is taken as a comment.
print(f"value_with_hash={dotenv.get_key(FILE, 'value_with_hash')}")
print(f"quoted_value_with_hash={dotenv.get_key(FILE, 'quoted_value_with_hash')}")
print(f"multi_line_value={dotenv.get_key(FILE, 'multi_line_value')}")
print(f"space_before_key={dotenv.get_key(FILE, 'space_before_key')}")
print(f"space_before_value={dotenv.get_key(FILE, 'space_before_value')}")
print(f"space_before_hash_value={dotenv.get_key(FILE, 'space_before_hash_value')}")
print(f"space_after_key={dotenv.get_key(FILE, 'space_after_key')}")
print(f"space_after_key_before_value={dotenv.get_key(FILE, 'space_after_key_before_value')}")
print(f"quoted_then_comment={dotenv.get_key(FILE, 'quoted_then_comment')}")
print(f"hash_with_spaces={dotenv.get_key(FILE, 'hash_with_spaces')}")
print(f"aa🐍bb={dotenv.get_key(FILE, 'aa🐍bb')}")
print(f"value_with_emoji={dotenv.get_key(FILE, 'value_with_emoji')}")

for i in range(12):
    key = f"sz{i}"
    print(f"len({key})={len(dotenv.get_key(FILE, key))}")

import jni
try:
    ArrayList = jni.cls("java/util/ArrayList")
except:
    print("SKIP")
    raise SystemExit

l = ArrayList()
print(l)
l.add("one")
l.add("two")

print(l.toString())
print(l)
print(l[0], l[1])

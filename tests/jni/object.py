import jni

try:
    Integer = jni.cls("java/lang/Integer")
except Exception:
    print("SKIP")
    raise SystemExit

# Create object
i = Integer(42)
print(i)
# Call object method
print(i.hashCode())
# Pass object to another method
System = jni.cls("java/lang/System")
System.out.println(i)

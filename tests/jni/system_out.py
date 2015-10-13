import sys
try:
    import jni
    System = jni.cls("java/lang/System")
except:
    print("SKIP")
    sys.exit()

System.out.println("Hello, Java!")

try:
    import jni

    System = jni.cls("java/lang/System")
except:
    print("SKIP")
    raise SystemExit

System.out.println("Hello, Java!")

def handle_exception(e, cmd, err):
  	if err and "Perhaps you forgot a comma?" not in str(e):
  	    print("Executing " + cmd + "...")
  	    print("Error did not mention a missing comma: " + str(e))
  	elif not err:
  	    print("Executing " + cmd + "...")
  	    print("Threw an error where none expected: " + str(e))
  	else:
  	    print(cmd + " executed as expected.")

def check(cmd, err):
    try:
        compile(cmd, 'single', 'exec')
        print(cmd + " executed as expected.")
    except NameError:
        try:
            exec(cmd)
            print(cmd + " executed as expected.")
        except Exception as e:
            handle_exception(e, cmd, err)
    except Exception as e:
        handle_exception(e, cmd, err)

check("[1,2,3,4,5]", False)
check("[1,2,3,4 5]", True)
check("(1,2,3,4 5)", True)
check("1,2,3,4 5", True)

def f(): pass
def g(): pass

check("f(), g()", False)
check("f() g()", True)
check("lambda x: f() g()", True)

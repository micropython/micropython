import system, term, sys, time, buttons

names = [
    "Renze Nicolai", "Joris Witteman", "Add more names here!!!!", "BADGE.TEAM",]

def action_exit(pushed):
    if (pushed):
        system.home()

def show_names():
    global names
    c = False
    y = 10

    term.header(True, "About")
    print("Developers:")
    for n in range(0, len(names)):
        if (names[n]==" "):
            break
        print(" - "+names[n])

def main():
    buttons.attach(buttons.BTN_HOME, action_exit)
    buttons.attach(buttons.BTN_MENU, action_exit)

    show_names()

main()

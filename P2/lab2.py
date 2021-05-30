class color:
    PURPLE = '\033[95m'
    CYAN = '\033[96m'
    DARKCYAN = '\033[36m'
    BLUE = '\033[94m'
    GREEN = '\033[92m'
    YELLOW = '\033[93m'
    RED = '\033[91m'
    BOLD = '\033[1m'
    UNDERLINE = '\033[4m'
    END = '\033[0m'


def switch(state, c):
    if state == 0:  # start state
        if c == 'a':
            return 1
        elif c == 'b':
            return 2
    elif state == 1:  # single a
        if c == 'a':
            return 3
        elif c == 'b':
            return 4
    elif state == 2:  # any number of b
        return 2 if c == 'b' else None
    elif state == 3:
        if c == 'a':
            return 3
        elif c == 'b':
            return 2
    elif state == 4:
        return 5 if c == 'b' else None
    elif state == 5:
        return 2 if c == 'b' else None


if __name__ == "__main__":
    data = str(input("Enter String:\n"))

    pretty_data = color.BOLD + data + color.END

    state = 0
    for c in data:
        state = switch(state, c)
        if state is None:
            print(pretty_data+color.RED+" not recognized."+color.END)
            exit(0)

    if state == 0 or state == 1 or state == 3:
        print(pretty_data+color.GREEN+" recognized under rule a*"+color.END)
    elif state == 4 or state == 2:
        print(pretty_data+color.GREEN+" recognized under rule a*b+"+color.END)
    elif state == 5:
        print(pretty_data+color.GREEN+" recognized under rule abb"+color.END)

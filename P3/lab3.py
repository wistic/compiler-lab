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


def getInput():
    try:
        production_count = int(input("Enter the count of productions: "))
    except:
        print("Invalid input.")
        exit(0)
    print(color.BLUE+color.BOLD+"[INFO]"+color.END+" Represent"+color.PURPLE+" epsilon" +
          color.END+" with " + color.END+color.PURPLE + "$"+color.END+" in the productions.")
    print(color.BLUE+color.BOLD+"[INFO]"+color.END+" For a production of the form " +
          color.PURPLE+"B -> cC|ϵ"+color.END+", enter it like "+color.PURPLE+"B->cC|$"+color.END)
    print(color.GREEN+"Enter the productions one per line:"+color.END)
    productions = dict()
    for i in range(production_count):
        lhs, rhs = str(input()).split("->", 1)
        lhs = lhs.strip()
        if lhs == "$":
            print(color.RED+"[ERROR] $ is reserved for epsilon."+color.END)
            exit(0)
        rhs = rhs.strip()
        rhs_set = {p.strip() for p in rhs.split("|")}
        rhs_set.discard('')
        if rhs_set:
            if lhs in productions:
                productions[lhs].update(rhs_set)
            else:
                productions[lhs] = rhs_set
    return productions


def first(productions: dict, lhs: str):
    if not lhs.isalpha() or lhs.islower():
        return {lhs}
    firstSet = set()
    for p in productions[lhs]:
        if p[0] == lhs:
            print(
                color.RED+"[ERROR] Left recursive grammar not allowed."+color.END)
            exit(0)

        index = 0
        while(index < (len(p)-1)):
            if p[index] != "$":
                break
            index += 1

        if p[index].isalpha():
            if p[index].islower():
                firstSet.add(p[index])
            else:
                while(index < (len(p)-1)):
                    recurseSet = first(productions, p[index])
                    if "$" not in recurseSet:
                        firstSet.update(recurseSet)
                        break
                    recurseSet.remove("$")
                    firstSet.update(recurseSet)
                    index += 1
                if index == (len(p)-1):
                    recurseSet = first(productions, p[index])
                    firstSet.update(recurseSet)
        else:
            firstSet.add(p[index])

    return firstSet


def printFirst(firstDict: dict):
    for key, value in firstDict.items():
        print("FIRST("+key+") = {"+", ".join(value)+"}")


def getFirst(productions: dict):
    firstDict = dict()
    for key in productions.keys():
        if key not in firstDict:
            firstDict[key] = first(productions, key)

    return firstDict


if __name__ == "__main__":
    productions = getInput()
    printFirst(getFirst(productions))

#coding=utf-8
import math

def math_map(v, al, ah, bl, bh):
    if al==ah:
        return bl
    if al > ah:
        al, ah = ah, al
    if v > ah:
        v = ah
    if v < al:
        v = al
    return bl + (bh - bl) * (v - al) / (ah - al)

def math_mean(myList):
    localList = [e for e in myList if type(e) == int or type(e) == float]
    if not localList: return
    return float(sum(localList)) / len(localList)

def math_median(myList):
    localList = sorted([e for e in myList if type(e) == int or type(e) == float])
    if not localList: return
    if len(localList) % 2 == 0:
        return (localList[len(localList) // 2 - 1] + localList[len(localList) // 2]) / 2.0
    else:
        return localList[(len(localList) - 1) // 2]

def math_modes(some_list):
    modes = []
    # Using a lists of [item, count] to keep count rather than dict
    # to avoid "unhashable" errors when the counted item is itself a list or dict.
    counts = []
    maxCount = 1
    for item in some_list:
        found = False
        for count in counts:
            if count[0] == item:
                count[1] += 1
                maxCount = max(maxCount, count[1])
                found = True
        if not found:
            counts.append([item, 1])
    for counted_item, item_count in counts:
        if item_count == maxCount:
            modes.append(counted_item)
    return modes

def math_standard_deviation(numbers):
    n = len(numbers)
    if n == 0: return
    mean = float(sum(numbers)) / n
    variance = sum((x - mean) ** 2 for x in numbers) / n
    return math.sqrt(variance)

def lists_sort(my_list, type, reverse):
    def try_float(s):
        try:
            return float(s)
        except:
            return 0
    key_funcs = {
        "NUMERIC": try_float,
        "TEXT": str,
        "IGNORE_CASE": lambda s: str(s).lower()
    }
    key_func = key_funcs[type]
    list_cpy = list(my_list)
    return sorted(list_cpy, key=key_func, reverse=reverse)

def format_content(mydict, cid):
    if 'lat' in mydict and 'long' in mydict:
        res = '{'+'"lat": "{}", "long": "{}", "clientid": "{}"'.format(mydict.pop('lat'),mydict.pop('long'),cid)
        if len(mydict)>0:
            res += ', "message": ['
            for d in mydict:
                res += '{{"label": "{}", "value": "{}"}},'.format(d,mydict[d])
            res = res[:-1] + "]"
        res += '}'
        return res
    else:
        print('Invalid Input')
        
def format_str(d):
    return str(d).replace("'",'"')
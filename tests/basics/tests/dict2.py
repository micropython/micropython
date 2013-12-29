# using strings as keys in dict

d = {'1': 1, '2': 2}
print(d['1'], d['2'])

d['3'] = 3
print(d['1'], d['2'], d['3'])

d['2'] = 222
print(d['1'], d['2'], d['3'])

try:
    import uheapq as heapq
except:
    try:
        import heapq
    except ImportError:
        print("SKIP")
        raise SystemExit

try:
    heapq.heappop([])
except IndexError:
    print("IndexError")

try:
    heapq.heappush((), 1)
except TypeError:
    print("TypeError")

def pop_and_print(h):
    l = []
    while h:
        l.append(str(heapq.heappop(h)))
    print(' '.join(l))

h = []
heapq.heappush(h, 3)
heapq.heappush(h, 1)
heapq.heappush(h, 2)
print(h)
pop_and_print(h)

h = [4, 3, 8, 9, 10, 2, 7, 11, 5]
heapq.heapify(h)
print(h)
heapq.heappush(h, 1)
heapq.heappush(h, 6)
heapq.heappush(h, 12)
print(h)
pop_and_print(h)

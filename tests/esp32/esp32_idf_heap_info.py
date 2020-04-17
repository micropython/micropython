try:
    import esp32
except ImportException:
    print("SKIP")
    raise SystemExit

# try getting heap regions, looks like a couple of small regions are always taken?
heap = esp32.idf_heap_info(esp32.MEM_DATA)
# print("heap:", heap)
if heap[1] == (7288, 0, 0, 0):
    print("Data has", heap[1])
else:
    print("OOPS, data doesn't have (7288, 0, 0, 0):", heap)

# try getting code regions, looks like a big region is always free?
code = esp32.idf_heap_info(esp32.MEM_EXEC)
# print("code:", code)
if code[0] == (15072, 15036, 15036, 15036):
    print("Code has", code[0])
else:
    print("OOPS, code doesn't have (15072, 15036, 15036, 15036):", code)

# try invalid param
print(esp32.idf_heap_info(-1))

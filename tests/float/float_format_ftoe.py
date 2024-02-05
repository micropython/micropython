# check a case where rounding was suppressed inappropriately when "f" was
# promoted to "e" for large numbers.
v = 8.888e32
print("%.2f" % v)  # '%.2f' format with e32 becomes '%.2e', expect 8.89e+32.

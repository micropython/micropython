for i in range(129):
    j = (1 << i)
    print(i, (j-1).bit_length(), (j).bit_length(), (j+1).bit_length())
    print(i, (-j-1).bit_length(), (-j).bit_length(), (-j+1).bit_length())

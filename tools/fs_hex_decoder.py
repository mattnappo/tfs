hex_str = ""
s = "".join([chr(int(hex_str[i:i+2], 16)) for i in range(0, len(hex_str), 2)])
print(s)


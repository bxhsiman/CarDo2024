import numpy as np

# Function to convert input string to list of lists of integers
def convert_input_to_matrix(input_str):
    input_str.replace(' ', '')
    rows = input_str.split('|')

    return [list(map(lambda x: int(x, 2), row.split(','))) for row in rows]

# Taking input for all 8 rows at once
input_data = """   0b00000100, 0b01000000, 0b00001000, 0b01111100, 0b00111111, 0b11110000, 0b11111000, 0b00100000, 
    0b00000100, 0b01000100, 0b01111111, 0b01001100, 0b00100000, 0b00010000, 0b01001001, 0b00100000, 
    0b01111100, 0b01011000, 0b01001001, 0b01011000, 0b00101100, 0b01010000, 0b01001001, 0b00111100, 
    0b00000100, 0b01100000, 0b01001001, 0b01001000, 0b00100010, 0b10010000, 0b01111111, 0b11100100, 
    0b00000100, 0b01000000, 0b01111111, 0b01000100, 0b00100001, 0b00010000, 0b00000101, 0b00110100, 
    0b00000100, 0b01000010, 0b01001001, 0b01101100, 0b00100010, 0b10010010, 0b11111101, 0b00101010, 
    0b11111100, 0b01000010, 0b01111111, 0b01010000, 0b01101100, 0b01001010, 0b01000101, 0b00100010, 
    0b01000100, 0b00111110, 0b01000001, 0b01000000, 0b11000000, 0b00000110, 0b00111000, 0b11111110,
"""
input_data = input_data.replace(' ', '')
rows = input_data.split(',\n')
rows.remove('')

rows = [row.replace('0b', '').replace(',', '') for row in rows]
# 把每一行倒序
# rows = [row[::-1] for row in rows]
# 把rows倒序
rows = rows[::-1]

print(rows)

hex_array = []
for i in range(len(rows[0])):
    buf = ''
    for row in rows:
        buf += row[i]
    # buf 转为hex
    hex_array.append(hex(int(buf, 2)))

hex_array = ', '.join(hex_array)
print(hex_array)
# Convert each value in the matrix to hexadecimal format
# hex_matrix = np.vectorize(hex)(matrix)

# # Print the hexadecimal matrix
# print(hex_matrix)

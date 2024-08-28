import numpy as np

# Function to convert input string to list of lists of integers
def convert_input_to_matrix(input_str):
    input_str.replace(' ', '')
    rows = input_str.split('|')

    return [list(map(lambda x: int(x, 2), row.split(','))) for row in rows]

# Taking input for all 8 rows at once
input_data = """    0b00010000, 0b11100000, 0b10000100, 0b11100000, 0b00011111, 0b11111100, 0b00011000, 0b01000000,
    0b00010100, 0b00100010, 0b01101100, 0b00100000, 0b00010000, 0b00000000, 0b00001100, 0b01000000,
    0b00010010, 0b00100110, 0b00111000, 0b00100000, 0b00010011, 0b11100000, 0b11110100, 0b01011110,
    0b10010001, 0b00101100, 0b01101100, 0b00100000, 0b00010010, 0b00100000, 0b00001110, 0b11100000,
    0b01110100, 0b00100000, 0b00100101, 0b11111110, 0b00010010, 0b00100000, 0b00011111, 0b11110000,
    0b00110011, 0b00000110, 0b11111100, 0b00110000, 0b11111111, 0b11111110, 0b00011111, 0b11110000, 
    0b00011001, 0b11111110, 0b00010000, 0b00001100, 0b00010010, 0b00100000, 0b00010001, 0b00010000, 
    0b00001000, 0b10000000, 0b00010000, 0b11111100, 0b00010010, 0b00100000, 0b00011111, 0b11110000, 
"""
input_data = input_data.replace(' ', '')
rows = input_data.split(',\n')
rows.remove('')

rows = [row.replace('0b', '').replace(',', '') for row in rows]

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

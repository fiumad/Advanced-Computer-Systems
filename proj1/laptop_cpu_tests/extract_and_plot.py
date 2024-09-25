import re
import os
import matplotlib.pyplot as plt
import numpy as np

def extract_data(file_path):
    #print("Extracting data from file: ", file_path)
    with open(file_path, 'r') as file:
        content = file.read()

        # Extract the number after './test.o'
        byte_size_match = re.search(r'\./test\.o\s+(\d+)', content)
        #print(byte_size_match)
        byte_size = int(byte_size_match.group(1)) if byte_size_match else None
        #print(byte_size)

        # Extract the floating point number before 'seconds time elapsed'
        time_elapsed_match = re.search(r'([\d.]+)\s+seconds time elapsed', content)
        time_elapsed = float(time_elapsed_match.group(1)) if time_elapsed_match else None

        return byte_size, time_elapsed

def plot_data_pairs(data_pairs):
    # Unpack the data pairs into two separate lists: one for byte size (x) and one for time elapsed (y)
    data_pairs.sort(key=lambda pair: pair[0])
    byte_sizes = [pair[0] for pair in data_pairs]
    time_elapsed = [pair[1] for pair in data_pairs]
    
    # Create a line plot plt.figure(figsize=(8, 6)) 
    plt.plot(byte_sizes, time_elapsed, color='blue', label='Time Elapsed vs Byte Size', linewidth=1.5)

    # Add labels and title
    plt.xlabel('Byte Size')
    plt.ylabel('Time Elapsed (seconds)')
    plt.suptitle('Intel i7-10850H - 50% Read Ratio')
    plt.xticks(byte_sizes)
    plt.xscale('log', base=2)
    plt.title('Performance: Byte Size vs Time Elapsed')
    plt.legend()

    # Show the grid and plot
    plt.grid(True)
    plt.show()

if __name__ == '__main__':

    data_pairs = []

    for filename in os.listdir("./50PERCENTrr/"):
        file_path = os.path.join("./50PERCENTrr/", filename)
        #print(file_path)

        if filename.endswith('.txt') and os.path.isfile(file_path) and filename != "CPUInfo.txt":
            result = extract_data(file_path)
            if result:
                data_pairs.append(result)

    plot_data_pairs(data_pairs)

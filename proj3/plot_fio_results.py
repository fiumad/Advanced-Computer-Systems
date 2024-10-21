import json
import os
import matplotlib.pyplot as plt

# Directory where FIO JSON output files are stored
output_dir = 'fio_results'

# Function to parse FIO JSON file and extract relevant data
def parse_fio_json(file_path):
    with open(file_path, 'r') as f:
        data = json.load(f)
    job = data['jobs'][0]  # Assuming a single job
    read_lat = job['read']['clat_ns']['mean'] / 1000  # Convert ns to µs
    write_lat = job['write']['clat_ns']['mean'] / 1000 if job['write']['iops'] > 0 else None
    iops = job['read']['iops'] if job['read']['iops'] > 0 else job['write']['iops']
    bw = job['read']['bw'] if job['read']['bw'] > 0 else job['write']['bw']  # in KiB/s
    return read_lat, write_lat, iops, bw

# Lists to hold results for each test category
block_sizes = []
block_latency = []
block_bandwidth = []

rw_ratios = []
rw_latency = []
rw_bandwidth = []

queue_depths = []
queue_latency = []
queue_bandwidth = []

# Process all JSON files and categorize based on filename
for file_name in os.listdir(output_dir):
    if file_name.endswith('.json'):
        file_path = os.path.join(output_dir, file_name)
        read_lat, write_lat, iops, bw = parse_fio_json(file_path)
        
        # Extract parameters from file name to categorize data
        if 'data-access-size' in file_name:
            block_size = file_name.split('_bs')[1].split('_')[0]
            block_sizes.append(block_size)
            block_latency.append(read_lat)
            block_bandwidth.append(iops)
        
        elif 'rw-ratio' in file_name:
            rw_ratio = file_name.split('_rw')[1].split('_')[0]
            rw_ratios.append(rw_ratio)
            rw_latency.append(read_lat)
            rw_bandwidth.append(iops)
        
        elif 'queue-depth' in file_name:
            queue_depth = file_name.split('_qd')[1].split('_')[0]
            queue_depths.append(queue_depth)
            queue_latency.append(read_lat)
            queue_bandwidth.append(iops)

# Convert KiB/s to MB/s for bandwidth plots
block_bandwidth = [bw / 1024 for bw in block_bandwidth]
rw_bandwidth = [bw / 1024 for bw in rw_bandwidth]
queue_bandwidth = [bw / 1024 for bw in queue_bandwidth]

# Function to generate and save plots
def plot_data(x, y, xlabel, ylabel, title, file_name):
    plt.figure(figsize=(8, 6))
    plt.plot(x, y, marker='o')
    plt.xlabel(xlabel)
    plt.ylabel(ylabel)
    plt.title(title)
    plt.grid(True)
    plt.savefig(f"{file_name}.png")
    plt.show()

# Plot 1: Effect of Data Access Size on Latency
plot_data(block_sizes, block_latency, 'Block Size', 'Latency (µs)',
          'Effect of Data Access Size on Latency', 'block_size_vs_latency')

# Plot 2: Effect of Data Access Size on Bandwidth
plot_data(block_sizes, block_bandwidth, 'Block Size', 'Bandwidth (MB/s)',
          'Effect of Data Access Size on Bandwidth', 'block_size_vs_bandwidth')

# Plot 3: Effect of Read/Write Ratio on Latency
plot_data(rw_ratios, rw_latency, 'Read/Write Ratio', 'Latency (µs)',
          'Effect of Read/Write Ratio on Latency', 'rw_ratio_vs_latency')

# Plot 4: Effect of Read/Write Ratio on Bandwidth
plot_data(rw_ratios, rw_bandwidth, 'Read/Write Ratio', 'Bandwidth (MB/s)',
          'Effect of Read/Write Ratio on Bandwidth', 'rw_ratio_vs_bandwidth')

# Plot 5: Effect of Queue Depth on Latency
plot_data(queue_depths, queue_latency, 'Queue Depth', 'Latency (µs)',
          'Effect of Queue Depth on Latency', 'queue_depth_vs_latency')

# Plot 6: Effect of Queue Depth on Bandwidth
plot_data(queue_depths, queue_bandwidth, 'Queue Depth', 'Bandwidth (MB/s)',
          'Effect of Queue Depth on Bandwidth', 'queue_depth_vs_bandwidth')


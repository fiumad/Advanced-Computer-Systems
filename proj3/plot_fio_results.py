import json
import os
import matplotlib.pyplot as plt

# Directory where FIO JSON output files are stored
output_dir = 'fio_results'

# Parameters used in the tests
block_sizes = ["4k", "16k", "32k", "128k"]
rwmix_ratios = [100, 70, 50, 0]  # New rwmix ratios
queue_depths = [1, 8, 64, 1024]

# Function to parse FIO JSON file and extract relevant data
def parse_fio_json(file_path):
    with open(file_path, 'r') as f:
        data = json.load(f)
    job = data['jobs'][0]  # Assuming a single job
    read_lat = job['read']['clat_ns']['mean'] / 1000  # Convert ns to µs
    write_lat = job['write']['clat_ns']['mean'] / 1000 if job['write']['iops'] > 0 else None
    #bw = job['read']['bw'] if job['read']['bw'] > 0 else job['write']['bw']  # in KiB/s
    if job['job options']['bs'] == '128k':
        if job['read']['bw'] > 0 and job['write']['bw'] > 0:
            bw = job['read']['bw'] + job['write']['bw']
            bw /= 2
        elif job['read']['bw'] > 0:
            bw = job['read']['bw']
        elif job['write']['bw'] > 0:
            bw = job['write']['bw']
    else:
        bw = 0
    if job['read']['iops'] > 0 and job['write']['iops'] > 0:
        iops = (job['read']['iops'] + job['write']['iops']) / 2
    elif job['read']['iops'] > 0:
        iops = job['read']['iops']
    elif job['write']['iops'] > 0:
        iops = job['write']['iops']

    return read_lat, write_lat, iops, bw

# Organize results by block size, rwmix ratio, and queue depth
results = {
    block_size: {
        rwmix: {
                qd: {'latency': None, 'bandwidth': None, 'iops': None} for qd in queue_depths
        } for rwmix in rwmix_ratios
    } for block_size in block_sizes
}

# Process all JSON files and categorize data by block size, rwmix ratio, and queue depth
for file_name in os.listdir(output_dir):
    if file_name.endswith('.json'):
        file_path = os.path.join(output_dir, file_name)
        read_lat, write_lat, iops, bw = parse_fio_json(file_path)
        
        # Extract parameters from file name
        block_size = file_name.split('_bs')[1].split('_')[0]
        rwmix = int(file_name.split('_rw')[1].split('_')[0])  # Now it's the rwmix ratio
        queue_depth = int(file_name.split('_qd')[1].split('_')[0].replace('.json', ''))

        # Debugging: Print extracted values to check for errors
        print(f"Processing file: {file_name}")
        print(f"Block Size: {block_size}, R/W Mix: {rwmix}, Queue Depth: {queue_depth}")

        # Store latency and bandwidth data
        if write_lat is None:
            results[block_size][rwmix][queue_depth]['latency'] = read_lat
        elif read_lat is None:
            results[block_size][rwmix][queue_depth]['latency'] = write_lat
        else:
            results[block_size][rwmix][queue_depth]['latency'] = (write_lat + read_lat) / 2  # Average of read and write lat
        results[block_size][rwmix][queue_depth]['bandwidth'] = bw / 1024  # Convert KiB/s to MB/s
        results[block_size][rwmix][queue_depth]['iops'] = iops

# Function to generate and save latency plots
def plot_latency(block_size):
    plt.figure(figsize=(10, 6))
    
    # Plot latency for each rwmix ratio across different queue depths
    for rwmix in rwmix_ratios:
        latencies = [results[block_size][rwmix][qd]['latency'] for qd in queue_depths]
        plt.plot(queue_depths, latencies, marker='o', label=f'R/W Mix {rwmix}')

    plt.xlabel('Queue Depth')
    plt.ylabel('Latency (µs)')
    plt.title(f'Latency for Block Size {block_size}')
    plt.legend(title='R/W Mix Ratio')
    plt.grid(True)
    plt.savefig(f"./plots/latency_block_size_{block_size}.png")
    plt.show()

# Function to generate and save bandwidth plots
def plot_bandwidth(block_size):
    plt.figure(figsize=(10, 6))
    
    # Plot bandwidth for each rwmix ratio across different queue depths
    for rwmix in rwmix_ratios:
        bandwidths = [results[block_size][rwmix][qd]['bandwidth'] for qd in queue_depths]
        plt.plot(queue_depths, bandwidths, marker='o', label=f'R/W Mix {rwmix}')

    plt.xlabel('Queue Depth')
    plt.ylabel('Bandwidth (MB/s)')
    plt.title(f'Bandwidth for Block Size {block_size}')
    plt.legend(title='R/W Mix Ratio')
    plt.grid(True)
    plt.savefig(f"./plots/bandwidth_block_size_{block_size}.png")
    plt.show()

def plot_iops(block_size):
    plt.figure(figsize=(10, 6))
    
    # Plot bandwidth for each rwmix ratio across different queue depths
    for rwmix in rwmix_ratios:
        iops = [results[block_size][rwmix][qd]['iops'] for qd in queue_depths]
        plt.plot(queue_depths, iops, marker='o', label=f'R/W Mix {rwmix}')

    plt.xlabel('Queue Depth')
    plt.ylabel('IOPS')
    plt.title(f'IOPS for Block Size {block_size}')
    plt.legend(title='R/W Mix Ratio')
    plt.grid(True)
    plt.savefig(f"./plots/IOPS_block_size_{block_size}.png")
    plt.show()

"""
# Generate plots for each block size
for bs in block_sizes:
    plot_latency(bs)
    plot_bandwidth(bs)
"""
for bs in block_sizes:
    if bs == "128k":
        plot_iops(bs)
    else:
        pass


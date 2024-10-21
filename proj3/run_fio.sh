#!/bin/bash

# Define parameters for the experiments
block_sizes=("4k" "64k" "512k")     # Three different block sizes for data access size tests
rw_ratios=("read" "randwrite" "rw") # Three read/write intensity ratios
queue_depths=(1 16 64)              # Three different I/O queue depths

# Directory for output
mkdir -p fio_results

# Function to run FIO with different parameters
run_fio_test() {
  name=$1
  block_size=$2
  rw=$3
  queue_depth=$4
  result_file="fio_results/${name}_bs${block_size}_rw${rw}_qd${queue_depth}.json"

  fio --name="$name" \
    --rw="$rw" \
    --bs="$block_size" \
    --iodepth="$queue_depth" \
    --size=2G \
    --numjobs=1 \
    --time_based --runtime=30 \
    --ioengine=libaio --direct=1 \
    --output-format=json \
    --allow_mounted_write=1 \
    --filename=/dev/nvme0n1p3 \
    --output="$result_file"

  echo "Test $name completed with Block Size=$block_size, RW=$rw, Queue Depth=$queue_depth"
}

# Experiment 1: Effect of Block Size on Latency and Bandwidth (Fix queue depth)
echo "Running experiments for Effect of Data Access Size on Latency and Bandwidth"
for bs in "${block_sizes[@]}"; do
  run_fio_test "data-access-size" "$bs" "randread" 16 # Fixed RW mode and queue depth
done

# Experiment 2: Effect of Read/Write Ratio on Latency and Bandwidth (Fix block size)
echo "Running experiments for Effect of Read/Write Ratio on Latency and Bandwidth"
for rw in "${rw_ratios[@]}"; do
  run_fio_test "rw-ratio" "64k" "$rw" 16 # Fixed block size and queue depth
done

# Experiment 3: Effect of Queue Depth on Latency and Bandwidth (Fix block size)
echo "Running experiments for Effect of I/O Queue Depth on Latency and Bandwidth"
for qd in "${queue_depths[@]}"; do
  run_fio_test "queue-depth" "64k" "randread" "$qd" # Fixed block size and RW mode
done

echo "All tests completed. Results are stored in 'fio_results' directory."

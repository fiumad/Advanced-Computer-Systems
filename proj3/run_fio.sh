#!/bin/bash

# Define 4 different values for each of the three parameters
block_sizes=("4k" "16k" "32k" "128k") # 4 different block sizes
rw_ratios=(100 70 50 0)               # 4 read/write intensity ratios
queue_depths=(1 8 64 1024)            # 4 different I/O queue depths

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
    --rw=randrw \
    --rwmixread="$rw" \
    --bs="$block_size" \
    --iodepth="$queue_depth" \
    --size=2G \
    --numjobs=1 \
    --time_based --runtime=30 \
    --ioengine=libaio --direct=1 \
    --allow_mounted_write=1 \
    --output-format=json \
    --filename=/dev/nvme0n1p3 \
    --output="$result_file"

  echo "Test $name completed with Block Size=$block_size, RW=$rw, Queue Depth=$queue_depth"
}

# Loop through all combinations of block size, read/write ratio, and queue depth
for bs in "${block_sizes[@]}"; do
  for rw in "${rw_ratios[@]}"; do
    for qd in "${queue_depths[@]}"; do
      run_fio_test "test_combination" "$bs" "$rw" "$qd"
    done
  done
done

echo "All 64 tests completed. Results are stored in 'fio_results' directory."

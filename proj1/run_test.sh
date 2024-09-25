#!/bin/bash

# Get CPU model
cpu_model=$(lscpu | grep "Model name" | awk -F: '{print $2}' | sed 's/^[ \t]*//;s/[ \t]*$//')

# Get today's date in YYYY-MM-DD format
today_date=$(date +%Y-%m-%d)

# Create a valid filename by replacing spaces in the CPU model with underscores
filename="${cpu_model// /_}${today_date}.txt"

# Run lscpu, remove the Vulnerabilities section, and save to file
lscpu | head -n 26

byte_size=1

# Final test should be 20
for i in {0..20}
  do
    test_file_name="./laptop_cpu_tests/${today_date}_test_${byte_size}.txt"
    byte_size=$((byte_size*2))
    echo "Current byte size: $byte_size"
    echo 0 > /proc/sys/kernel/nmi_watchdog
    perf stat ./test.o $byte_size 25 2> $test_file_name
    echo 1 > /proc/sys/kernel/nmi_watchdog
  done

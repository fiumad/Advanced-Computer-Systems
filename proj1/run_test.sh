#!/bin/bash

# Get CPU model
cpu_model=$(lscpu | grep "Model name" | awk -F: '{print $2}' | sed 's/^[ \t]*//;s/[ \t]*$//')

# Get today's date in YYYY-MM-DD format
today_date=$(date +%Y-%m-%d)

# Create a valid filename by replacing spaces in the CPU model with underscores
filename="${cpu_model// /_}${today_date}.txt"

# Run lscpu, remove the Vulnerabilities section, and save to file

# Inform the user where the file is saved
echo "lscpu output saved to $filename"

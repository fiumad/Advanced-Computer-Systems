#!/bin/bash

echo 0 > /proc/sys/kernel/nmi_watchdog
perf stat -e cache-references,cache-misses,L1-dcache-loads,L1-dcache-load-misses,LLC-loads,LLC-load-misses ./cache_miss_rate.o hostile 2> hostile_test.txt
perf stat -e cache-references,cache-misses,L1-dcache-loads,L1-dcache-load-misses,LLC-loads,LLC-load-misses ./cache_miss_rate.o friendly 2> friendly_test.txt
echo 1 > /proc/sys/kernel/nmi_watchdog

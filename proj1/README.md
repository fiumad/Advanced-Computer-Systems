# Advanced Computer Systems - Project 1
## Results

### Experiment 1: Memory Latency
1. The latency of the main memory is **78.6	ns** as found by Intel's Memory Latency Checker using the --idle_latency test.

### Experiment 2: Memory Bandwidth
#### Byte size vs. elapsed time for writing 1GB of data then reading 1 GB * read ratio of data.
![byteSizeVSelapsed.png](./laptop_cpu_tests_exp2/byteSizeVSelapsed100rr.png)
![byteSizeVSelapsed.png](./laptop_cpu_tests_exp2/byteSizeVSelapsed75rr.png)
![byteSizeVSelapsed.png](./laptop_cpu_tests_exp2/byteSizeVSelapsed50rr.png)
![byteSizeVSelapsed.png](./laptop_cpu_tests_exp2/byteSizeVSelapsed25rr.png)

## Method

1. To characterize the main memory latency when queue length is zero, I ... 
2. To determine the bandwidth of the main memory, I wrote a program that allocates 1 Gigabyte of memory and reads/writes to that memory in a loop. I measured the time it took to write then immediately read the same data from memory using perf. The runtime of the program vs. the size of the data being read/written is shown in the graph above. The maximum bandwidth is achieved at larger byte sizes. Since the program wrote AND read 1 GB of data in the time it took to run the entire program (in the case of 100% RR), the bandwidth calculation is as follows:

```bash
2GB / 0.352861789 seconds = 5.67 GB/s (for 100% read ratio)
```

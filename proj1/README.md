# Advanced Computer Systems - Project 1

## Results

### Byte size vs. elapsed time for writing then reading 1 GB of data.
![byteSizeVSelapsed.png](./laptop_cpu_tests/byteSizeVSelapsed.png)

## Method

1. To characterize the main memory latency when queue length is zero, I ... 
2. To determine the bandwidth of the main memory, I wrote a program that allocates 1 Gigabyte of memory and reads/writes to that memory in a loop. I measured the time it took to write then immediately read data to memory using perf. The runtime of the program vs. the size of the data being read/written is shown in the graph above. The maximum bandwidth is achieved at larger byte sizes. Since the program wrote AND read 1 GB of data in the time it took to run the entire program, the bandwidth calculation is as follows:

```bash
2GB / 0.352861789 seconds = 5.67 GB/s
```


##### How to Run (notes to self)
```bash
sudo perf stat ./test.o <byte_size>
```

# Project 3: SSD Performance Profiling

## How it Works
1. First, a 2GB partition is created on the SSD. For my laptop, this was `/dev/nvme0n1p3`
2. Next, mount this partition using `mount -t /dev/nvme0n1p3 ./fio_partition/`
3. Once the partition is mounted, executing `./run.sh` will run the fio benchmark on the SSD
4. The results are stored in `./fio_results/` in the json format
5. These results are then parsed and plotted using `plot_fio_results.py`
6. The resulting plots are saved in `./plots/`
### Hardware
The following tests were performed on the SAMSUNG MZVLB512HBJQ-000L7 NVME SSD.

The example enterprise SSD in the assignment PDF achieved IOPS of 130k for a 100% write test with 4k block size.
The SSD in my laptop achieves 225k IOPS for the same test. I believe this unexpected difference in performance is most likely due to redundancy and error correction technologies that are present in the enterprise grade device that aren't included in a consumer grade device such as mine. This would result in the enterprise grade device having lower performance but higher reliability.

### FIO Options
As seen in `run_fio.sh`, the FIO tool is run according to the following (where `$name` is the name of the test, `$rw` is the read/write ratio, `$block_size` is the block size, `$queue_depth` is the queue depth, and `$result_file` is the output file):
```bash
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
```
As seen above, all tests are run with the "randrw" mode meaning reads and writes are not sequential (i.e. worst case performance). Each job is run for 30 seconds, using the desired combination of R/W ratio, block size, and queue depth, and the results are output in json format.

## Results
The results are two types of plots - the first being latency plots, the second being IOPS plots. There are four variations of each type of plot, one for each block size. Each plot varies queue depth on the x-axis. As per the rubric and assignment handout, the experiments with small data access sizes (up to 64k block size) are measured in IOPS, then they switch to bandwidth plots in MB/s (IOPS plot is included in 128k block size section for easier comparisons to previous tests).

### 4k Block Size
![4K Block Size, Latency Plot](./plots/latency_block_size_4k.png)

![4K Block Size, IOPS Plot](./plots/IOPS_block_size_4k.png)

We see in the plots above that as the queue depth increases, the latency drastically increases. This is expected as the SSD is being bombarded with more requests than it can handle. When more requests are added to the queue, each request has to wait longer. The throughput plot shows that as the SSD receives more requests, the throughput increases until it begins to plateau. This is expected since the SSD will always be fed the next request when the queue is full (not waiting around for a new request), but once it reaches its maximum speed, the queue getting longer will not increase the throughput.

We also see that the throughput at 100% read or 100% write is higher than a mix of both. There are several reasons for this. First, pure read or pure write workloads can allow the SSD's hardware to be fully allocated to a single task. We also notice that pure read workloads are faster than pure write workloads. One reason for this could be that modern SSD's have extra processing to do on write operations to manage wear leveling and extend the life of the device. This introduces extra overhead that diminishes write speeds.

### 16k Block Size

![16K Block Size, Latency Plot](./plots/latency_block_size_16k.png)
![16K Block Size, IOPS Plot](./plots/IOPS_block_size_16k.png)

As we increase the block size to 16k, we see that our SSD performance follows the same trends as with 4k block size. One key difference however is that our latencies are much higher, now reaching as much as 4x the latencies of 4k block size. Our throughput also sees a marked decrease by more than half of the throughput we saw at a 4k block size.

### 32k Block Size
![32K Block Size, Latency Plot](./plots/latency_block_size_32k.png)
![32K Block Size, IOPS Plot](./plots/IOPS_block_size_32k.png)

Once again our data continues to follow the same trends we saw from the past two block sizes examined. Our latencies once again increased greatly, and our throughputs roughly halved.

### 128k Block Size

![128K Block Size, Latency Plot](./plots/latency_block_size_128k.png)
![128K Block Size, IOPS Plot](./plots/IOPS_block_size_128k.png)
![128K Block Size, Bandwidth Plot](./plots/bandwidth_block_size_128k.png)

Finally, here we see that our latencies are at their highest and our throughputs are at their lowest once converted to IOPS. (~10000 peak IOPS)

## Conclusion
By the automated use of the FIO tool, we experimentally observe several key relationships regarding SSD performance. First, as queue depth increases, the latency of each individual request increases. Simultaneously, with increased queue depth, the throughput of the SSD will increase as the device no longer sits idle in between requests. Eventually, the throughput of the SSD plateaus with increased queue depth as the SSD reaches its throughput limit.

We also observe that increasing the data access size both lowers the 





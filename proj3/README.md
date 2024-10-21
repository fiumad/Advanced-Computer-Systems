# Project 3: SSD Performance Profiling

## How it Works
1. First, a 2GB partition is created on the SSD. For my laptop, this was `/dev/nvme0n1p3`
2. Next, mount this partition using `mount -t /dev/nvme0n1p3 ./fio_partition/`
3. Once the partition is mounted, executing `./run.sh` will run the fio benchmark on the SSD
4. The results are stored in `./fio_results/` in the json format
5. These results are then parsed and plotted using `plot_fio_results.py`
6. The resulting plots are saved in `./plots/`

import subprocess
import matplotlib.pyplot as plt
import re

def measure_runtime(binary_path, input_file, num_threads):
    try:
        # Use subprocess to invoke the shell with time
        result = subprocess.run(
            f"time {binary_path} {input_file} {num_threads}",
            shell=True, stderr=subprocess.PIPE, stdout=subprocess.DEVNULL, text=True
        )

        # Extract the real time (elapsed time) from stderr output
        stderr = result.stderr

        # Use regular expression to match the 'real' time part (e.g. 'real 0m2.345s')
        match = re.search(r"real\s+(\d+)m(\d+\.\d+)s", stderr)
        if match:
            minutes = float(match.group(1))
            seconds = float(match.group(2))
            runtime = minutes * 60 + seconds  # Convert to seconds
            return runtime
        else:
            print(f"Could not parse time for {num_threads} threads: {stderr}")
            return None

    except Exception as e:
        print(f"Error running with {num_threads} threads: {e}")
        return None

def main():
    binary_path = "./trie.o"
    input_file = "Column.txt"
    max_threads = 24
    times = []
    thread_counts = range(1, max_threads + 1)

    for num_threads in thread_counts:
        print(f"Running with {num_threads} threads...")
        runtime = measure_runtime(binary_path, input_file, num_threads)
        if runtime is not None:
            times.append(runtime)
        else:
            times.append(float('nan'))  # Handle failed runs gracefully

    # Plot the results
    plt.figure(figsize=(10, 6))
    plt.plot(thread_counts, times, marker='o')
    plt.title("Execution Time vs. Number of Threads")
    plt.suptitle("Trie Encoding Speed Test")
    plt.xlabel("Number of Threads")
    plt.ylabel("Execution Time (seconds)")
    plt.grid(True)
    plt.savefig("trie_execution_time_plot.png")
    plt.show()

if __name__ == "__main__":
    main()


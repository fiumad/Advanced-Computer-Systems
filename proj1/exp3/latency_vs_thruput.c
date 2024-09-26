#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <time.h>
#include <sys/time.h>

#define BUFFER_SIZE 100000000  // 100 MB buffer size
#define NUM_THREADS 8          // Number of concurrent threads
#define OPERATIONS_PER_THREAD 10000000 // Number of memory operations per thread

int *buffer;  // Main memory buffer

typedef struct {
    int thread_id;
    int operations;
} thread_data_t;

void* memory_access(void* arg) {
    thread_data_t* data = (thread_data_t*)arg;
    int id = data->thread_id;
    int operations = data->operations;

    struct timeval start, end;
    long elapsed_time = 0;
    
    for (int i = 0; i < operations; i++) {
        // Random read/write access in the buffer
        int index = rand() % BUFFER_SIZE;
        
        // Record the time before the access
        gettimeofday(&start, NULL);
        
        // Read-modify-write operation (write after read)
        int value = buffer[index];
        buffer[index] = value + id;
        
        // Record the time after the access
        gettimeofday(&end, NULL);
        
        // Calculate time per access
        elapsed_time += ((end.tv_sec - start.tv_sec) * 1000000L + end.tv_usec) - start.tv_usec;
    }

    // Return the total time spent in microseconds
    long* result = malloc(sizeof(long));
    *result = elapsed_time;
    return result;
}

int main() {
    // Allocate the buffer
    buffer = (int*)malloc(BUFFER_SIZE * sizeof(int));

    // Initialize the buffer with some values
    for (int i = 0; i < BUFFER_SIZE; i++) {
        buffer[i] = i;
    }

    // Thread variables
    pthread_t threads[NUM_THREADS];
    thread_data_t thread_data[NUM_THREADS];
    long total_time = 0;
    long operations_performed = 0;

    // Create and launch threads
    for (int num_threads = 1; num_threads <= NUM_THREADS; num_threads++) {
        total_time = 0;
        operations_performed = 0;
        printf("\nRunning with %d thread(s)\n", num_threads);

        for (int t = 0; t < num_threads; t++) {
            thread_data[t].thread_id = t;
            thread_data[t].operations = OPERATIONS_PER_THREAD;

            pthread_create(&threads[t], NULL, memory_access, (void*)&thread_data[t]);
        }

        // Join threads and gather results
        for (int t = 0; t < num_threads; t++) {
            long* elapsed_time;
            pthread_join(threads[t], (void**)&elapsed_time);
            total_time += *elapsed_time;
            operations_performed += thread_data[t].operations;
            free(elapsed_time);
        }

        // Calculate average latency and throughput
        double avg_latency = (double)total_time / operations_performed;
        double throughput = (double)operations_performed / (total_time / 1000000.0);

        printf("Average Latency: %.2f microseconds\n", avg_latency);
        printf("Throughput: %.2f operations/second\n", throughput);
    }

    // Free the buffer
    free(buffer);
    return 0;
}


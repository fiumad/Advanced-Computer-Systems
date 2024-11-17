#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>
#include "helper.h"

#define MAX_LINE_LENGTH 128
#define MAX_LINES 200000000

typedef struct {
    char **lines;
    int total_lines;
    int *current_line;
    pthread_mutex_t *lock;
    HashTable *hash_table; // Shared hash table
    int thread_id;
} ThreadData;

void *process_lines(void *arg) {
    ThreadData *data = (ThreadData *)arg;

    while (1) {
        pthread_mutex_lock(data->lock);
        if (*(data->current_line) >= data->total_lines) {
            pthread_mutex_unlock(data->lock);
            break;
        }
        int line_index = (*(data->current_line))++;

        // Hash the line into the hash table
        insert(data->hash_table, data->lines[line_index], line_index);
        pthread_mutex_unlock(data->lock);
        if (line_index % 1000000 == 0){
           printf("Thread %d hashed line %d: %s\n", data->thread_id, line_index, data->lines[line_index]);
        }
    }

    return NULL;
}

int main(int argc, char *argv[]) {
    if (argc != 4) {
        fprintf(stderr, "Usage: %s <file_path> <num_threads>\n", argv[0]);
        return EXIT_FAILURE;
    }

    if (argv[3] == "save") {
      printf("Saving hash table to file\n");

      char *file_path = argv[1];
      int num_threads = atoi(argv[2]);

      if (num_threads <= 0) {
          fprintf(stderr, "Number of threads must be greater than 0.\n");
          return EXIT_FAILURE;
      }

      FILE *file = fopen(file_path, "r");
      if (!file) {
          perror("Error opening file");
          return EXIT_FAILURE;
      }

      // Allocate memory dynamically for lines
      char **lines = malloc(MAX_LINES * sizeof(char *));
      if (!lines) {
          perror("Error allocating memory for lines");
          fclose(file);
          return EXIT_FAILURE;
      }

      int total_lines = 0;
      char buffer[MAX_LINE_LENGTH];
      while (fgets(buffer, MAX_LINE_LENGTH, file)) {
          buffer[strcspn(buffer, "\n")] = '\0';
          lines[total_lines] = strdup(buffer);
          if (!lines[total_lines]) {
              perror("Error duplicating line");
              fclose(file);
              return EXIT_FAILURE;
          }
          total_lines++;
      }
      fclose(file);

      int current_line = 0;
      pthread_mutex_t lock;
      pthread_mutex_init(&lock, NULL);

      // Initialize the hash table
      HashTable *hash_table = create_hash_table();

      pthread_t threads[num_threads];
      ThreadData thread_data[num_threads];

      for (int i = 0; i < num_threads; i++) {
          thread_data[i].lines = lines;
          thread_data[i].total_lines = total_lines;
          thread_data[i].current_line = &current_line;
          thread_data[i].lock = &lock;
          thread_data[i].hash_table = hash_table;
          thread_data[i].thread_id = i;

          if (pthread_create(&threads[i], NULL, process_lines, &thread_data[i]) != 0) {
              perror("Error creating thread");
              return EXIT_FAILURE;
          }
      }

      for (int i = 0; i < num_threads; i++) {
          printf("Joining thread %d\n", i);
          pthread_join(threads[i], NULL);
      }

      // Print message after all threads are done
      printf("All lines have been hashed into the hash table.\n");

      // Save the hash table to a file
      save_hash_table_to_file(hash_table, "encoded_data.txt");

      pthread_mutex_destroy(&lock);
    }
    else if (argv[3] == "read"){
      *HashTable hash_table;
      hash_table = load_hash_table_from_file("encoded_data.txt");
    }

    char search_term[128];

    while (search_term[0] != '!') {
      printf("Enter a search term, enter \'!\' to end the program: ");
      scanf("%128s", search_term);
      const char *search_query = search_term;
      printf("Searching for key: %s\n", search_query);
      Node *query;
      query = search(hash_table, search_query);

      if (!query) {
        printf("Key not found in hash table.\n");
      }
      else {
        printf("Key: %s, Counter: %d, Indexes: ", query->key, query->counter);
        for (int i = 0; i < query->index_list.size; i++) {
          printf("%d ", query->index_list.indices[i]);
        }
        printf("\n");
      }
    }

    // Free memory
    for (int i = 0; i < total_lines; i++) {
        free(lines[i]);
    }
    free(lines);
    free_hash_table(hash_table);

    return EXIT_SUCCESS;
}

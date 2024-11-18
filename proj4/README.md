# Project 4: Encoding Column Data

## How it Works

1. The raw column data is ingested into a carefully selected data structure
(explained below).
2. The data is then written to a text file with the following form:

  ```plaintext
  daniel 3 8787 28823 1226268
  ```

  The first entry is the key, the second entry is the number of occurrances
  of this key in the original column data, and all following entries are
  the indices at which the key can be found.

3. Once this file is created, it can be later loaded for speedy searching either
by searching for the entire key, or by searching for a prefix.

## Data Structure

![Trie](./Trie.png)

*In the image above, the prefix "dan" is found during search (red nodes) and each key with the same prefix is found by traversing
through all of the prefix's children nodes*

The data structure selected to store the encoded column data is a ![Trie](https://en.wikipedia.org/wiki/Trie).
Each node in the trie represents a single letter. As the column data is encoded, as an
entire key is encoded, the tree grows as each letter is processed. This means that all
keys in the data with the same prefix share the same nodes until the point at which they
diverge. This impressive storage efficiency as well as constant speed searches (both prefix and word).

## Results and Analysis
The trie data structure is incredibly efficient at storing and searching for keys. Its drawback is
that it is slow to encode the raw column data into the trie. This is because it cannot easily be parallelized without
risk of resource contention. Unfortunately, multithreading the encoding process actually led to slower raw data ingest as
the threads were constantly waiting on each other to access the trie. However, the trie is incredibly fast at searching for
both words and prefixes.

![Encoding Time](./trie_execution_time_plot.png)

As seen in the graph above, the trie encoding time does not see a speedup as more threads are used. In fact it seems that the overhead of orchestrating
multiple threads slows down the time to encode the raw data into the trie.

This deficiency is met with nearly instantaneous search times.

With over 200,000,000 keys in the trie, the search time for a single word is measured as 0.000000 seconds.
The time to search for all keys with a 3 letter prefix was 0.000074 seconds.

```plaintext
Time taken to search by prefix "dan": 0.000074 seconds
Time taken to search for word "dano": 0.000000 seconds
key dano has 315 occurrances
```

SIMD is not implemented as no speedup is necessary with this choice of data structure. I supposed that many keys could searched for simultaneously
and that there could be a large speedup from SIMD for this workload; however, the assignment refers to search speedup during single key or single prefix searches.

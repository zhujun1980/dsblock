# dsblock — Data Structures in File Block

[English](README.md) | [简体中文](README.zh-CN.md)

**dsblock** is a C++14 library of read-only data structures that live in a serialized
binary file and are queried directly from memory via `mmap`. The same file can be
mapped (and shared) across processes, reloaded automatically when it changes on disk,
and queried concurrently from multiple threads.

It currently ships two structures — a **Bloom filter** and a **Ternary Search Tree
(TST)** — exposed through a stable **C API** and a thin **Python (ctypes)** binding.

## Features

- **Memory-mapped** — data is read straight from an `mmap`'d file; no deserialization step.
- **Shared memory** — the same binary file maps into multiple processes.
- **Auto-reload** — mappings detect when the backing file changes (mtime/size) and reload.
- **Thread-safe access** — the multi-threaded file cache (`FCacheMT`) guards lookups and uses
  `shared_ptr` so in-flight queries stay valid across a reload.
- **Memory efficient** — TST nodes are stored as a flat array addressed by index, not pointers.
- **Fast search** — prefix, near-neighbor (edit distance), and in-text matching.
- **UTF-8 support** — keys and text are processed per UTF-8 code point.
- **Python friendly** — `import dsblock` and call the helpers.

## Data Structures

### Bloom Filter
Probabilistic set-membership testing, backed by a bit set and MurmurHash3, with an
embedded data-version tag.

Python unit tests: [`bloomfilter_test.py`](python/dsblock/bloomfilter_test.py)

### Ternary Search Tree (TST)
A [ternary search tree](https://en.wikipedia.org/wiki/Ternary_search_tree) supporting:

- **Dictionary lookup** — exact membership.
- **Prefix search** — all words sharing a prefix.
- **Near-neighbor search** — all words within a given edit distance.
- **In-text match** — find dictionary words occurring inside a longer text (C API).

Python unit tests: [`tst_test.py`](python/dsblock/tst_test.py)

## Usage (Python)

### Prefix Search

```python
from dsblock.tst import tst_create_bin_from_txt, tst_prefix_search

# txt_file: one word per line
# bin_file: the serialized TST binary
tst_create_bin_from_txt(txt_file, bin_file)   # build the binary from the text file
ret = tst_prefix_search(bin_file, "李")        # mmap the binary and search by prefix
print(ret)
```

Output:

```json
{
    "total_words": 7,
    "return_words": 7,
    "read_size": 55,
    "words": ["李会昌", "李俭", "李光辉", "李桐", "李浩", "李践", "李韦"]
}
```

### Near-Neighbor Search

```python
from dsblock.tst import tst_near_neighbors_search

# return words whose edit distance to the key is <= 1
ret = tst_near_neighbors_search(bin_file, "张三丰", 1)
print(ret)
```

Output:

```json
{
    "total_neighbors": 3,
    "return_neighbors": 3,
    "read_size": 30,
    "neighbors": ["张一丰", "张二丰", "赵三丰"]
}
```

See [`tst_test.py`](python/dsblock/tst_test.py) for more examples.

### Bloom Filter

```python
from dsblock.bloomfilter import (
    bloomfilter_create_bin_from_txt,
    bloomfilter_is_present,
    bloomfilter_get_stats,
)

bloomfilter_create_bin_from_txt(
    txt_file, bin_file,
    num_elements=100000, data_vers=1, hash_count=7, bits_per_element=10,
)
print(bloomfilter_is_present(bin_file, "李会昌"))   # True / False
print(bloomfilter_get_stats(bin_file))             # {bit_size, byte_size, hashs_cnt, data_vers}
```

## C API

The full ABI lives in [`src/c_api.h`](src/c_api.h). Highlights:

```c
/* Build */
int TstCreateBinaryFromText(const char* txt_file, size_t txt_file_len,
                            const char* bin_file, size_t bin_file_len);

/* Query */
int TstIsPresent(const char* bin_file, size_t bin_file_len,
                 const char* key, size_t key_len);

int TstPrefixSearch(const char* bin_file, size_t bin_file_len,
                    const char* prefix, size_t prefix_len, int with_prefix,
                    char* out_buffer, size_t* len, int* total);

int TstNearNeighborSearch(const char* bin_file, size_t bin_file_len,
                          const char* word, size_t word_len, int distance,
                          char* out_buffer, size_t* len, int* total);

/* Find dictionary words inside a longer text (caller must free the result) */
CTstMatchedWords* TstMatch(const char* bin_file, size_t bin_file_len,
                           const char* text, size_t text_len);
const char*       GetMatchedWords(CTstMatchedWords* words);
void              FreeTstMatchedWords(CTstMatchedWords* words);
```

`*PrefixSearch` / `*NearNeighborSearch` write NUL-separated UTF-8 words into
`out_buffer`; on return `*len` holds the bytes written, `*total` the number of
matches found (which may exceed what fit in the buffer), and the return value is
the count actually written.

## Architecture

```
Python (ctypes)   python/dsblock/*.py
      │
C API             src/c_api.{h,cc}                stable extern "C" boundary
      │
C++ structures    src/tst.cc  src/bloom_filter.cc
      │
Infrastructure    fc.h (file cache)  file_mapping.h (mmap)  shmm_posix.cc
                  murmur_hash3 / utf8 / bit_set / log / file_utils
```

- **Index-based nodes.** A `TstNode` stores child links as integer offsets, so the
  whole node array can be `mmap`'d and shared with zero deserialization — `Load()`
  just reinterprets the mapped address.
- **Binary layout.** `[uint16 magic][uint64 size][uint64 capacity][TstNode array]`.
- **Balanced build.** `Build()` sorts the input and inserts via binary-search midpoints
  (`BuildFromMiddle`) for a balanced tree; node capacity is precomputed from UTF-8
  common-prefix lengths.
- **File cache.** `FCacheMT` keys mappings by a MurmurHash of the path, checks mtime/size
  on every access, and reloads transparently when the file changes.

## Build

```shell
git clone --recursive https://github.com/zhujun1980/dsblock
cd dsblock
mkdir build && cd build
cmake ..
make -j
```

This produces a shared library (`libdsblock.so` / `libdsblock.dylib`) and a static
library (`libdsblock.a`). Requirements: CMake ≥ 3.0, a C++14 compiler, POSIX `mmap`.

### Run the tests

```shell
# C++ unit tests (GoogleTest)
cd build && ctest

# Python tests
python3 ../python/setup.py test
```

## Project Layout

```
src/            C++ sources and the C API
python/dsblock/ Python ctypes binding and tests
test/           GoogleTest unit tests (googletest as a submodule)
CMakeLists.txt  top-level build
```

## License

See the repository for license details.

Data Structure in File Block
============================

* Shared memory
* Auto reload on files changed
* Memory efficient
* Fast search
* UTF-8 supported
* Python friendly
* C API

## bloomfilter

python unittest cases: [bloomfilter_test.py](python/dsblock/bloomfilter_test.py)

## Tst (Ternary search tree)

[Ternary search tree](https://en.wikipedia.org/wiki/Ternary_search_tree)

* Dictionary
* Prefix Search
* Near Neighbors Search

python unittest cases [tst_test.py](python/dsblock/tst_test.py)

## Build

```
mkdir build && cd build
cmake ..
make -j
```

```
# test
python3 ../python/setup.py test
```

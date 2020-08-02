Data Structure in File Block
============================

* Shared memory
* Auto reload on files changed
* Memory efficient
* Fast search
* UTF-8 supported
* Python friendly
* C API

## Bloomfilter

python unittest cases: [bloomfilter_test.py](python/dsblock/bloomfilter_test.py)

## Tst (Ternary search tree)

[Ternary search tree](https://en.wikipedia.org/wiki/Ternary_search_tree)

* Dictionary
* Prefix Search
* Near Neighbors Search

### Prefix Search

```python
# txt_file: one word one line
# bin_file: tst binary file

tst_create_bin_from_txt(txt_file, bin_file) # create binary file from txt file
ret = tst_prefix_search(bin_file, "李")     # map binary file to shared memory and search by prefix
print(ret)
```

__Output:__

```json
{
    "total_words": 7,
    "return_words": 7,
    "read_size": 55,
    "words": ["李会昌", "李俭", "李光辉", "李桐", "李浩", "李践", "李韦"]
}
```

### Near Neighbors Search

```python
ret = tst_near_neighbors_search(bin_file, "张三丰", 1) # search the words which distance less than 1 to key
print(ret)
```

__Output:__

```json
{
    "total_neighbors": 3,
    "return_neighbors": 3,
    "read_size": 30,
    "neighbors": ["张一丰", "张二丰", "赵三丰"]
}
```

go to python unittest cases for more detail [tst_test.py](python/dsblock/tst_test.py)

## Build

```shell
git clone --recursive https://github.com/zhujun1980/dsblock
cd dsblock
mkdir build && cd build
cmake ..
make -j
```

```shell
# test
python3 ../python/setup.py test
```


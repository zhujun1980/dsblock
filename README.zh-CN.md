# dsblock — 文件块中的数据结构

[English](README.md) | [简体中文](README.zh-CN.md)

**dsblock** 是一个 C++14 库，提供一系列**只读数据结构**：它们被序列化到二进制文件中，
通过 `mmap` 直接映射到内存进行查询。同一个文件可以在多个进程间映射（并共享），
在磁盘文件变更时自动重载，并支持多线程并发查询。

目前内置两种结构——**布隆过滤器（Bloom Filter）** 和 **三叉搜索树（Ternary Search Tree, TST）**，
对外通过稳定的 **C API** 和轻量的 **Python（ctypes）绑定** 暴露。

## 特性

- **内存映射** —— 数据直接从 `mmap` 的文件读取，无需反序列化步骤。
- **共享内存** —— 同一个二进制文件可映射进多个进程。
- **自动重载** —— 映射会检测后端文件的变化（mtime/大小）并重新加载。
- **线程安全** —— 多线程文件缓存 `FCacheMT` 对查询加锁，并使用 `shared_ptr`，
  使得正在进行中的查询在重载期间依然有效。
- **内存高效** —— TST 节点以「索引寻址的扁平数组」存储，而非指针。
- **快速查询** —— 支持前缀搜索、近邻搜索（编辑距离）、文本内匹配。
- **UTF-8 支持** —— 键和文本按 UTF-8 码点处理。
- **Python 友好** —— `import dsblock` 即可调用。

## 数据结构

### 布隆过滤器（Bloom Filter）
基于位集合（bit set）和 MurmurHash3 的概率型集合成员判断，内嵌数据版本号（data version）。

Python 单元测试：[`bloomfilter_test.py`](python/dsblock/bloomfilter_test.py)

### 三叉搜索树（TST）
一种[三叉搜索树](https://en.wikipedia.org/wiki/Ternary_search_tree)，支持：

- **字典查询** —— 精确成员判断。
- **前缀搜索** —— 查找共享某前缀的所有词。
- **近邻搜索** —— 查找与给定词编辑距离不超过 N 的所有词。
- **文本内匹配** —— 在一段较长文本中找出其中出现的词典词（C API）。

Python 单元测试：[`tst_test.py`](python/dsblock/tst_test.py)

## 使用方法（Python）

### 前缀搜索

```python
from dsblock.tst import tst_create_bin_from_txt, tst_prefix_search

# txt_file：每行一个词
# bin_file：序列化后的 TST 二进制文件
tst_create_bin_from_txt(txt_file, bin_file)   # 从文本文件构建二进制文件
ret = tst_prefix_search(bin_file, "李")        # 将二进制文件 mmap 后按前缀搜索
print(ret)
```

输出：

```json
{
    "total_words": 7,
    "return_words": 7,
    "read_size": 55,
    "words": ["李会昌", "李俭", "李光辉", "李桐", "李浩", "李践", "李韦"]
}
```

### 近邻搜索

```python
from dsblock.tst import tst_near_neighbors_search

# 返回与 key 编辑距离 <= 1 的词
ret = tst_near_neighbors_search(bin_file, "张三丰", 1)
print(ret)
```

输出：

```json
{
    "total_neighbors": 3,
    "return_neighbors": 3,
    "read_size": 30,
    "neighbors": ["张一丰", "张二丰", "赵三丰"]
}
```

更多示例见 [`tst_test.py`](python/dsblock/tst_test.py)。

### 布隆过滤器

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

完整 ABI 见 [`src/c_api.h`](src/c_api.h)，核心接口：

```c
/* 构建 */
int TstCreateBinaryFromText(const char* txt_file, size_t txt_file_len,
                            const char* bin_file, size_t bin_file_len);

/* 查询 */
int TstIsPresent(const char* bin_file, size_t bin_file_len,
                 const char* key, size_t key_len);

int TstPrefixSearch(const char* bin_file, size_t bin_file_len,
                    const char* prefix, size_t prefix_len, int with_prefix,
                    char* out_buffer, size_t* len, int* total);

int TstNearNeighborSearch(const char* bin_file, size_t bin_file_len,
                          const char* word, size_t word_len, int distance,
                          char* out_buffer, size_t* len, int* total);

/* 在较长文本中查找词典词（结果需由调用方释放） */
CTstMatchedWords* TstMatch(const char* bin_file, size_t bin_file_len,
                           const char* text, size_t text_len);
const char*       GetMatchedWords(CTstMatchedWords* words);
void              FreeTstMatchedWords(CTstMatchedWords* words);
```

`*PrefixSearch` / `*NearNeighborSearch` 会把以 NUL 分隔的 UTF-8 词写入 `out_buffer`；
返回时 `*len` 为已写入字节数，`*total` 为匹配总数（可能超过缓冲区能容纳的数量），
函数返回值为实际写入的词数。

## 架构

```
Python (ctypes)   python/dsblock/*.py
      │
C API             src/c_api.{h,cc}                稳定的 extern "C" 边界
      │
C++ 数据结构      src/tst.cc  src/bloom_filter.cc
      │
基础设施          fc.h（文件缓存）  file_mapping.h（mmap）  shmm_posix.cc
                  murmur_hash3 / utf8 / bit_set / log / file_utils
```

- **基于索引的节点。** `TstNode` 用整型偏移而非指针存储子节点链接，因此整个节点数组
  可直接 `mmap` 并共享，零反序列化——`Load()` 仅对映射地址做 reinterpret。
- **二进制布局。** `[uint16 魔数][uint64 size][uint64 容量][TstNode 数组]`。
- **平衡构建。** `Build()` 先对输入排序，再以二分中点方式插入（`BuildFromMiddle`）以构建
  平衡树；节点容量由 UTF-8 公共前缀长度预先精确计算。
- **文件缓存。** `FCacheMT` 以路径的 MurmurHash 为键缓存映射，每次访问检查 mtime/大小，
  文件变更时透明重载。

## 构建

```shell
git clone --recursive https://github.com/zhujun1980/dsblock
cd dsblock
mkdir build && cd build
cmake ..
make -j
```

会生成共享库（`libdsblock.so` / `libdsblock.dylib`）和静态库（`libdsblock.a`）。
依赖：CMake ≥ 3.0、支持 C++14 的编译器、POSIX `mmap`。

### 运行测试

```shell
# C++ 单元测试（GoogleTest）
cd build && ctest

# Python 测试
python3 ../python/setup.py test
```

## 目录结构

```
src/            C++ 源码与 C API
python/dsblock/ Python ctypes 绑定与测试
test/           GoogleTest 单元测试（googletest 为子模块）
CMakeLists.txt  顶层构建文件
```

## 许可证

许可证详情见仓库。

Data Structure in File Block
============================

* 基于共享内存
* 自动加载变更
* 内存紧凑
* 查找高效
* 支持 UTF-8
* Python 友好
* C API，便于移植

## bloomfilter

* 集合查找

python测试用例: [bloomfilter_test.py](python/dsblock/bloomfilter_test.py)

## Tst（Triple Search Tries）

* 字典查找
* 前缀查找
* 近邻查找

python测试用例: [tst_test.py](python/dsblock/tst_test.py)

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

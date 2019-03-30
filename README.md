# CSS-tree

[![Build Status](https://travis-ci.org/gvinciguerra/CSS-tree.svg?branch=master)](https://travis-ci.org/gvinciguerra/CSS-tree)

Single-header C++11 implementation of the Cache Sensitive Search tree (CSS-tree) described in:

> Rao, J., & Ross, K. A. (1998). _Cache conscious indexing for decision-support in main memory_.

## Minimal example

```c++
std::vector<int32_t> data = {-3, 2, 4, 11, 35, 60};
CSSTree<64, int32_t> tree(data); // 64 is the block size of the tree in bytes
*tree.find(11); // == 11
tree.find(100); // == tree.end()
```

## Running tests

```
git clone https://github.com/gvinciguerra/CSS-tree.git
cd CSS-tree
cmake . -DCMAKE_BUILD_TYPE=Release
make
./test/tests
```

## License

This project is licensed under the terms of the MIT License.


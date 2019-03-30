/*
Copyright (c) 2019 Giorgio Vinciguerra

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

#pragma once

#include <vector>
#include <cstdint>
#include <cassert>
#include <algorithm>
#include <stdexcept>

/**
 * A static (read-only) multiway tree stored implicitly, without pointers.
 *
 * The implementation is derived from the paper:
 * Rao, J., & Ross, K. A. (1998). Cache conscious indexing for decision-support in main memory.
 *
 * @tparam NodeSize the size in bytes of a node
 * @tparam K the type of the elements in the container
 */
template<size_t NodeSize, typename K = int64_t>
class CSSTree {
    static_assert(NodeSize >= sizeof(K), "");

    size_t tree_height;
    size_t half_marker;
    size_t n_internal_nodes;
    std::vector<K> tree;
    const std::vector<K> leaves;
    const size_t slots_per_node = NodeSize / sizeof(K);

    template<class Iterator>
    inline typename std::vector<K>::const_iterator find_in_leaves(Iterator lo, Iterator hi, K key) const {
        for (; lo != hi && *lo < key; ++lo);
        return key == *lo ? lo : end();
    }

public:

    /**
     * Constructs the container with the copy of the contents of data, which must be sorted.
     * @param data the vector to be used as source to initialize the elements of the container with
     */
    explicit CSSTree(const std::vector<K> &data) : leaves(data) {
        if (!std::is_sorted(data.begin(), data.end()))
            throw std::invalid_argument("Data must be sorted");

        const auto n = leaves.size();
        const auto leaf_nodes = ceil(n / double(slots_per_node));
        tree_height = size_t(ceil(log(leaf_nodes) / log(slots_per_node + 1)));
        const auto expp = size_t(pow(slots_per_node + 1, tree_height));
        const auto last_internal_node = size_t((expp - leaf_nodes) / slots_per_node);
        n_internal_nodes = (size_t) ((expp - 1) / slots_per_node) - last_internal_node;
        tree = std::vector<K>(n_internal_nodes * slots_per_node);
        half_marker = (expp - 1) / slots_per_node;

        size_t i = tree.size();
        while (i-- > 0) {
            auto node = i / slots_per_node;
            auto child = node * (slots_per_node + 1) + 1 + i % slots_per_node;
            while (child < n_internal_nodes) // follow rightmost branch
                child = child * (slots_per_node + 1) + slots_per_node + 1;

            // child is a leaf -> map it to an index in the tree
            long diff = (child - half_marker) * slots_per_node;
            if (diff < 0)
                tree[i] = leaves[diff + n + slots_per_node - 1];
            else if (diff + slots_per_node - 1 < n - last_internal_node * slots_per_node)
                tree[i] = leaves[diff + slots_per_node - 1];
            else
                // special case: fill ancestor of the last leaf node with the
                // last element of (the biggest in) the first half of the tree
                tree[i] = leaves[n - last_internal_node * slots_per_node - 1];
        }
    }

    /**
     * Finds an element with key equivalent to key.
     * @param key key value of the element to search for
     * @return an iterator to an element with key equivalent to key. If no such element is found, past-the-end iterator
     *         is returned
     */
    inline typename std::vector<K>::const_iterator find(K key) const {
        if (n_internal_nodes == 0)
            return find_in_leaves(leaves.cbegin(), leaves.cend(), key);

        size_t child = 0;
        while (child < n_internal_nodes) {
            auto index_in_tree = child * slots_per_node;
            if (NodeSize > 256) { // use binary search for large pages and scan for smaller ones
                auto lo = tree.cbegin() + index_in_tree;
                auto hi = std::min(tree.cend(), lo + slots_per_node + 1);
                auto pos = std::lower_bound(lo, hi, key);
                if (pos == hi)
                    --pos;
                child = child * (slots_per_node + 1) + 1 + std::distance(lo, pos);
            } else {
                size_t lo;
                for (lo = 0; lo < slots_per_node && tree[index_in_tree + lo] < key; ++lo);
                child = child * (slots_per_node + 1) + 1 + lo;
            }
        }

        long diff = (long(child) - long(half_marker)) * slots_per_node;
        if (diff < 0)
            diff += leaves.size();
        assert(diff >= 0);

        auto lo = leaves.cbegin() + std::min(leaves.size(), size_t(diff));
        auto hi = leaves.cbegin() + std::min(leaves.size(), diff + slots_per_node);
        return find_in_leaves(lo, hi, key);
    }

    /**
     * Returns an iterator to the first element of the container; that is, the first leaf element.
     * @return an iterator to the first element
     */
    typename std::vector<K>::const_iterator begin() const {
        return leaves.cbegin();
    }

    /**
     * Returns an iterator to the element following the last element of the container.
     * @return an iterator to the element following the last element
     */
    typename std::vector<K>::const_iterator end() const {
        return leaves.cend();
    }

    /**
     * Returns the size in bytes of all the internal nodes in the tree.
     * @return the size in bytes of the internal nodes
     */
    size_t size_in_bytes() const {
        return tree.size() * sizeof(K);
    }

    /**
     * Returns the height of the tree.
     * @return the height of the tree
     */
    size_t height() const {
        return tree_height;
    }

    /**
     * Returns the number of elements in the container.
     * @return the number of elements in the container
     */
    size_t size() const {
        return leaves.size();
    }

};

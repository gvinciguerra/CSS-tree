//
// Created by Giorgio Vinciguerra on 2019-03-30.
//

#define CATCH_CONFIG_MAIN

#include "catch.hpp"
#include "csstree.hpp"
#include <vector>
#include <random>
#include <algorithm>

TEST_CASE("collection methods") {
    std::vector<int32_t> data = {-3, 2, 4, 11, 35, 60};
    CSSTree<32, int32_t> css(data);

    SECTION("input sorted") {
        try {
            CSSTree<32, int32_t> tree({2, 1, 0});
            REQUIRE(false);
        } catch (std::exception &e) {

        }
    }

    SECTION("begin/end") {
        REQUIRE(std::is_sorted(css.begin(), css.end()));
        REQUIRE(std::equal(css.begin(), css.end(), data.cbegin()));
    }

    SECTION("size") {
        REQUIRE(css.size() == data.size());
        REQUIRE(css.size_in_bytes() == 0);
        REQUIRE(css.height() == 0);
    }

    SECTION("size2") {
        CSSTree<1, int8_t> tree({1, 2, 3, 4, 5});
        REQUIRE(tree.size() == 5);
        REQUIRE(tree.size_in_bytes() == 4);
    }
}

TEST_CASE("find") {
    CSSTree<2, int16_t> css({1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17});
    REQUIRE(*css.find(8) == 8);
    REQUIRE(*(css.find(16) + 1) == 17);
    REQUIRE(*(css.find(13) - 2) == 11);
    REQUIRE(css.find(42) == css.end());
    REQUIRE(css.find(-1) == css.end());
}

TEST_CASE("large data") {
    std::vector<uint32_t> data(1000000);
    std::generate(data.begin(), data.end(), std::rand);
    std::sort(data.begin(), data.end());
    CSSTree<64, uint32_t> css(data);
    for (auto key : data)
        REQUIRE(*css.find(key) == key);
    REQUIRE(css.find(data.back() + 100) == css.end());
}
// The MIT License (MIT)
//
// Copyright (c) 2021 NVIDIA CORPORATION & AFFILIATES
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.

#include <catch2/catch.hpp>
#include <iostream>

#include "src/utils/utils.h"

namespace triton { namespace backend { namespace dali { namespace test {

TEST_CASE("Split string") {
  using namespace std;
  vector<vector<string>> ref = {
      {},
      {},
      {},
      {"This is a string"},
      {"This is a string"},
      {"This is a string"},
      {"This is ", "a string"},
      {"This is ", "a string"},
      {"This is ", "a string"},
      {"This is a string"},
      {"This is a string"},
      {"This is a string"},
      {"Th", "is is a str", "ing"},
      {"This is ", "a string"},
  };

  SECTION("Single-character delimiter") {
    vector<string> inputs = {
        "",
        ":",
        "::",
        "This is a string",
        ":This is a string",
        "This is a string:",
        "This is :a string",
        ":This is :a string",
        "This is :a string:",
        ":This is a string:",
        "::This is a string",
        "This is a string::",
        "Th:is is a str:ing",
        "This is ::a string",
    };

    REQUIRE(inputs.size() == ref.size());
    for (size_t i = 0; i < inputs.size(); i++) {
      REQUIRE(ref[i] == split(inputs[i], ":"));
    }
  }

  SECTION("Multi-character delimiter") {
    vector<string> inputs = {
        "",
        "()",
        "()()",
        "This is a string",
        "()This is a string",
        "This is a string()",
        "This is ()a string",
        "()This is ()a string",
        "This is ()a string()",
        "()This is a string()",
        "()()This is a string",
        "This is a string()()",
        "Th()is is a str()ing",
        "This is ()()a string",
    };

    REQUIRE(inputs.size() == ref.size());
    for (size_t i = 0; i < inputs.size(); i++) {
      REQUIRE(ref[i] == split(inputs[i], "()"));
    }
  }
}

}}}}  // namespace triton::backend::dali::test

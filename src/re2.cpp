//
// server.cpp
// ~~~~~~~~~~
//
// Copyright (c) 2003-2018 Christopher M. Kohlhoff (chris at kohlhoff dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
#include <re2/re2.h>
#include <assert.h>
#include <iostream>

int main() {
  std::cout << RE2::FullMatch("hello", "h.*o");
  assert(!RE2::FullMatch("hello", "e"));

  assert(RE2::PartialMatch("hello", "h.*o"));
  assert(RE2::PartialMatch("hello", "e"));

  return 0;
}
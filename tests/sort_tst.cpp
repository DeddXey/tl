//
// Created by alex on 22.09.2020.
//
#include "doctest.h"
#include "sort/qsort.h"
#include <cstring>
#include <vector>

TEST_CASE("sort, sort_quick_recursive")
{
  char buffer[] = "das ist fantastisch!";
  auto  seq    = tl::makeRange(buffer, strlen(buffer));

  tl::sort_quick_recursive(seq);

  auto prev = seq.begin();
  MESSAGE(buffer);

  for (auto i = seq.begin() + 1; i != seq.end(); ++i) {
    CHECK(*prev <= *i);
    prev = i;
  }

  std::vector<char>buffer1  { 1,2,3,4,5,6 };

  tl::sort_quick_recursive(buffer1);

  auto prev1 = buffer1.begin();
  MESSAGE(buffer1);
  for (auto i = buffer1.begin() + 1; i != buffer1.end(); ++i) {
      CHECK(*prev1 <= *i);
    prev1 = i;
  }


}
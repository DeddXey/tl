//
// Created by alex on 22.09.2020.
//

#ifndef TL_QSORT_H
#define TL_QSORT_H

#include <algorithm>
#include "ranges/range.h"

namespace tl {
template<typename T>
void sort_quick_recursive(T &&seq)
{
  if ((seq.end() - seq.begin()) == 1) {
    return;
  }
  if ((seq.end() == seq.begin()) ) {
    return;
  }
  auto pivot = seq.end() - 1;
  auto i     = seq.begin();
  auto j     = seq.end() - 1;

  while (j != i) {
    for (; i != j; ++i) {
      if (*i > *pivot) {
        break;
      }
    }

    for (; j != i; --j) {
      if (*j < *pivot) {
        break;
      }
    }
    std::swap(*i, *j);
  }
  std::swap(*j, *pivot);

  sort_quick_recursive(Range(seq.begin(), j));
  sort_quick_recursive(Range(j + 1, seq.end()));
}

} // namespace tl

#endif // TL_QSORT_H

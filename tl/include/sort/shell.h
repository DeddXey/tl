//
// Created by alex on 22.09.2020.
//

#ifndef TL_SHELL_H
#define TL_SHELL_H

#include <algorithm>

namespace tl {
template<typename T>
void sort_shell(T &&seq)
{

  for (auto step = (seq.end() - seq.begin()) / 2;
       step > 0;
       step /= 2) {

    for (auto i = seq.begin(), j = i + step; j != seq.end(); ++i, ++j) {
      if (*i > *j)
        std::swap(*i, *j);
    }
  }
}
}



#endif // TL_SHELL_H

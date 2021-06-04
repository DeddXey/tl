#ifndef TL_DELAYER_H
#define TL_DELAYER_H

#include <cstddef>
#include <array>

template <typename T, size_t size>
class delayer {
  std::array<T, size> buffer;
  bool isReset = true;
  size_t  ptr = 0;

public:
  delayer()
  {
    reset();
  }
  void reset()
  {
    isReset = true;
    std::fill(buffer.begin(), buffer.end(), 0);
    ptr = 0;
  }

  T iteration(const T& sample)
  {
    if (isReset) {
      isReset = false;
      std::fill(buffer.begin(), buffer.end(), sample);
    }

    auto out = buffer[ptr];
    buffer[ptr] = sample;
    ptr++;
    if (ptr == (size) ) {
      ptr = 0;
    }
    return out;
  }
};


#endif // TL_DELAYER_H

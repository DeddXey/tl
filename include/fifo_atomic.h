#ifndef FIFO_ATOMIC_H
#define FIFO_ATOMIC_H

//#include "dbg.h"
#include <array>
#include <atomic>
#include <mutex>
#include <optional>


namespace tl {








/// Atomic realization of ring fifo buffer
template<typename T, uint32_t sz>
class fifo_atomic
{
//  friend class fifo_read_iterator<fifo_atomic<T, sz>>;
public:
  using value_type [[maybe_unused]] = T;
  constexpr static uint32_t depth   = sz;

    /// Iterator for fifo ring buffer
    class iterator;
    friend class iterator;
    class iterator:public std::iterator<
            std::random_access_iterator_tag,
            T>
    {
        fifo_atomic<T, sz>*     fifo_;
        uint32_t idx;

        template<typename R>
        union value {
            T       value;
            uint8_t buff[sizeof(T)];
        };

    public:
//        using value_type [[maybe_unused]] = F::value_type;

        iterator(): idx(0) {}

        explicit iterator(fifo_atomic<T, sz>& fifo,
                                    uint32_t index): fifo_(&fifo), idx(index){}

        bool operator == (const iterator& other) const
        {
            return this->idx == other.idx;
        }

        bool operator != (const iterator& other) const
        {
            return !(*this == other);
        }

        value_type& operator*()
        {
            return fifo_->get_by_idx(idx);
        }

        iterator& operator++()
        {
            ++idx;
            if (idx == depth) {
                idx = 0;
            }
            return *this;
        }

        const iterator operator++(int)
        {
            iterator temp = *this;
            ++(*this);
            return temp;
        }

        iterator operator + (int val)
        {
            iterator temp = *this;
            temp.idx += val;
            if (idx >= depth) {
                idx -= depth;
            }
            return temp;
        }

        iterator operator - (int val)
        {
            iterator temp = *this;
            int32_t  tmp_idx = temp.idx - val;
            if (tmp_idx < 0) {
                tmp_idx = depth + tmp_idx;
            }
            temp.idx = tmp_idx;
            return temp;
        }

        uint32_t operator - (iterator other)
        {
            int out = this->idx - other.idx;
            if (out < 0) {
                out = depth + out;
            }

            return out;
        }

        /// Get LE value
        template<typename V>
        std::optional<V> get_value_le()
        {
            value<V> val;
            uint8_t  cnt = 0;
            auto current = *this;
            do {
                if (current != fifo_->end()) {
                    val.buff[cnt] = *(current);
                    ++current;
                }
                else {
                    return std::nullopt;
                }
                ++cnt;
            } while (cnt < sizeof(T));

            *this = current;
            return std::optional<T>{ val.value };
        }

        /// Get BE value from byte buffer
        template<typename V>
        std::optional<V> get_value_be()
        {
            value<V> val;
            uint8_t  cnt = 0;
            auto current = *this;
            do {
                if (current != fifo_->end()) {
                    val.buff[sizeof(T) - 1 - cnt] = *current++;
                }
                else {
                    return std::nullopt;
                }
                ++cnt;
            } while (cnt < sizeof(T));
            *this = current;
            return std::optional<T>{ val.value };
        }

        /// get uint8 value from buffer

        std::optional<uint8_t> get_u8_le()
        {
            if (*this != fifo_->end()) {
                uint8_t val = *static_cast<uint8_t *>(&(**this));
                ++(*this);
                return std::optional<uint8_t>{ val };
            }
            return std::nullopt;
        }
        /// get int8 value from buffer
        std::optional<int8_t> get_i8_le()
        {
            if (*this != fifo_->end()) {
                int8_t val = *static_cast<int8_t *>(&(**this));
                ++(*this);
                return std::optional<int8_t>{ val };
            }
            return std::nullopt;
        }

    };






protected:
  uint32_t              writeIndex = 0;
  uint32_t              readIndex  = 0;
  std::atomic<uint32_t> used       = 0;
  std::array<T, sz>     data;

  T& get_by_idx(const uint32_t idx)
  {
    return data[idx];
  }

  [[nodiscard]] uint32_t get_read_idx() const
  {
    return readIndex;
  }
  [[nodiscard]] uint32_t get_write_idx() const
  {
    return writeIndex;
  }

public:

  /// Get begin read iterator
  iterator begin()
  {
    iterator out (*this, readIndex);
    return out;
  }

  /// Get begin write iterator
  iterator end()
  {
    uint32_t temp = readIndex + used;
    if (temp >= sz) {
      temp -= sz;
    }

    iterator    out (*this, temp);
    return out;
  }

  fifo_atomic() : used(0)
  {
    clear();
  }

  void clear()
  {
    writeIndex = readIndex = 0;
    used.store(0);
  }

  [[nodiscard]] bool empty() const
  {
    return (used.load() == 0);
  }

  [[nodiscard]] bool full() const
  {
    return (used.load() == sz - 1);
  }

  [[nodiscard]] uint32_t size() const
  {
    return (sz - used.load());
  }

  [[nodiscard]] constexpr uint32_t maxsize() const
  {
    return sz;
  }

  bool push(const T &value)
  {
    uint32_t try_used = used.load();
    do {
      if (try_used == sz - 1)
        return false;

    } while (!used.compare_exchange_weak(try_used, try_used + 1));

    data[writeIndex] = value;

    ++writeIndex;
    if (writeIndex == sz)
      writeIndex = 0;

    return true;
  }

  /// \brief Remove last read element from fifo
  void pop()
  {
    uint32_t try_used = used.load();
    do {
      if (try_used == 0)
        return;

    } while (!used.compare_exchange_weak(try_used, try_used - 1));

    ++readIndex;
    if (readIndex == sz)
      readIndex = 0;
  }
  /// \brief Remove last read element from fifo
  void pop(uint32_t num)
  {
    uint32_t try_used = used.load();
    do {
      if (try_used < num)
        return;

    } while (!used.compare_exchange_weak(try_used, try_used - num));

    readIndex += num;
    if (readIndex >= sz)
      readIndex -= sz;
  }



  std::optional<const T> getOut() const
  {
    return used.load() == 0 ? std::nullopt
                            : std::optional<const T>{ data[readIndex] };
  }

  T& getOutMut() const
  {
    return get_by_idx(readIndex);
  }

  uint32_t getReadIndex()
  {
    return readIndex;
  }

  uint32_t getWriteIndex()
  {
    return writeIndex;
  }

  uint32_t getUsed()
  {
    return used;
  }

  ///
  /// \brief Get fifo element to write
  /// \return ref to  element
  ///
  T &asyncGetToWrite()
  {
    return data[writeIndex];
  }

  ///
  /// \brief Write complete
  /// \return ref to new write element
  ///
  std::optional<T>asyncWritten()
  {

    uint32_t try_used = used.load(std::memory_order_seq_cst);
    do {
      if (try_used == sz - 1)
        return {};

    } while (!used.compare_exchange_weak(try_used,
                                         try_used + 1));

    ++writeIndex;
    if (writeIndex == sz) {
      writeIndex = 0;
    }

    return std::optional<T>{ data[writeIndex] };
  }
};
} // namespace tl
#endif // FIFO_ATOMIC_H

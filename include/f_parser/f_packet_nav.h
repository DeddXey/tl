#pragma once

#include <cstdint>
#include "ranges/range.h"
#include "bufferdescriptor.h"
#include "f_mavlink.h"
#include "f_parse_result.h"

//#include "value.h"

template<typename T>
class packet_rc_nav
{

public:
  constexpr static uint8_t signature = 0xAC;
  constexpr static uint8_t num_channels = 4;

#pragma pack(push, 1)
  struct packet_nav {
    uint8_t signature_;
    uint8_t id;
    uint16_t channels[num_channels];
    uint16_t crc;
  };
#pragma pack(pop)

  explicit packet_rc_nav(T* context_)
    : context(context_)
  {
    binary_packet = BufferDescriptor<uint8_t>(entity);
  }
  [[nodiscard]] constexpr size_t size() const
  {
      return sizeof(packet_nav);
  }

  inline auto fill()
  {
    auto ret = [&](auto ran) -> parse_result<decltype(ran)> {
        parser prs(ran);

        for (int i = 0; i < num_channels; ++i) {
          context->analog_channels[i] =
            prs.template get_value_le<uint16_t>().value();
//          con.debug() << "v:" << context->analog_channels[i] << "\n";
        }
        return parse_result(prs.get_range());

    };
    return ret;
  }

  [[nodiscard]] BufferDescriptor<uint8_t> &packet_buffer()
  {
    entity.signature_ = signature;
    entity.id = context->id;
    for (int i = 0; i < num_channels; ++i) {
      entity.channels[i] = context->analog_channels[i];
    }
    entity.crc = crc_mav(binary_packet.begin() + 1, binary_packet.end() - 2);
    return binary_packet;
  }

protected:
  T* context;
  packet_nav entity;
  BufferDescriptor<uint8_t> binary_packet;
};

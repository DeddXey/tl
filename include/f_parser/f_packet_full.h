#pragma once

#include "bufferdescriptor.h"
#include "f_mavlink.h"
#include "parser/parse_result.h"
#include "ranges/range.h"
#include "value.h"
#include <cstdint>
#include <optional>

/// 9  2 = 18 bytes = 144 bits
/// 2400 => 16 Hz
template<typename T>
class packet_rc_full
{

public:
  constexpr static uint8_t signature = 0xAB;
  constexpr static uint8_t num_channels = 6;

#pragma pack(push, 1)
  struct packet_full {
    uint8_t signature_;
    uint8_t id;
    uint16_t channels[num_channels];
    uint16_t digital;
    uint16_t crc;
  };
#pragma pack(pop)

    explicit packet_rc_full(T* context_)
            : context(context_) {
        binary_packet = BufferDescriptor<uint8_t>(entity);
    }

    [[nodiscard]] constexpr size_t size() const {
        return sizeof(packet_full);
    }

    auto fill() {

        auto ret = [&](auto ran) -> parse_result<decltype(ran)>  {
          parser prs(ran);

          for (int i = 0; i < num_channels; ++i) {
            context->analog_channels[i] =
              prs.template get_value_le<uint16_t>().value();
//                      con.debug() << "v:" << context->analog_channels[i] << "\n";
          }
          context->digital_channels = prs.template get_value_le<uint16_t>().value();

//          con.debug() <<  "\n";
          return parse_result(prs.get_range());

        };
        return ret;
    }

    [[nodiscard]] BufferDescriptor<uint8_t> &packet_buffer() {
        entity.signature_ = signature;
        entity.id = context->id;
        for (int i = 0; i < num_channels; ++i) {
            entity.channels[i] = context->analog_channels[i];
        }
        entity.digital = context->digital_channels;
        entity.crc = crc_mav(binary_packet.begin() + 1, binary_packet.end() - 2);
        return binary_packet;
    }

protected:
  T* context;
  packet_full entity;
  BufferDescriptor<uint8_t> binary_packet;
};
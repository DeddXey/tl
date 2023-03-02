#pragma once

#include "f_mavlink.h"
#include "f_parse_result.h"
#include "ranges/range.h"
#include <cstdint>
#include <optional>
#include "bufferdescriptor.h"

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
            auto d = this;
            auto curr = ran.begin();
            for (uint16_t &i: d->context->analog_channels) {
                i = curr.template get_value_le<uint16_t>().value();
                //      ++ran.begin();
            }
            d->context->digital_channels = curr.template get_value_le<uint16_t>().value();

            return parse_result(tl::Range(curr, ran.end()));
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
#ifndef REG_PACKETBUFFER_H
#define REG_PACKETBUFFER_H

#include "fifo_atomic.h"
#include <cstdint>

namespace  tl {

class fifo_read_iterator
{

};


template<uint32_t sz>
class packet_buffer: public fifo_atomic<uint8_t , sz>
{




  uint8_t get_u8();
  int8_t get_i8();
  uint16_t get_u16();
  int16_t get_i16();
  uint32_t get_u32();
  int32_t get_i32();
  float get_float();

};

}



#endif // REG_PACKETBUFFER_H

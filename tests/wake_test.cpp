#include "doctest.h"
#include "fifo_atomic.h"
#include "wake/wake.h"
#include <cstdint>

const uint8_t packet1[]{
    0xC0, //FEND
    0x10, //addr
    0x11, //cmd
    0x3,//length
    0x81, // data 0
    0x82, // data 0
    0x83, // data 0
    0x99 //crc
};


TEST_CASE("wake::parse_frame_no_crc") {
  wake::parser                parser;
  tl::fifo_atomic<uint8_t, 3> fifo;

  for (auto item: packet1) {
    fifo.push(item);
    parser.parse(fifo, [](auto header, auto data) {
               CHECK_EQ(0x10, header.address);
               CHECK_EQ(0x11, header.command);
               CHECK_EQ(0x03, header.data_length);
               CHECK_EQ(0x81, data[0]);
               CHECK_EQ(0x81, data[1]);
               CHECK_EQ(0x83, data[2]);
             }
    );
  }
}




TEST_CASE("wake::send") {

  std::array<uint8_t, 256> data;
  std::array<uint8_t, 256> out;  //    uint8_t out[256];
  uint8_t out_cnt = 0;


  auto sendfun = [&](auto b)  {
    out[out_cnt] = b;
    ++out_cnt;
  };

  wake::frame::send(
    0x82,
    0x33,
    0x4,
    data,
    sendfun,
    false);

  CHECK_EQ(wake::frame::fend, out[0]);
  CHECK_EQ(0x82, out[1]);
  CHECK_EQ(0x33, out[2]);
  CHECK_EQ(0x4, out[3]);

}

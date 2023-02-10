//#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "doctest.h"
#include <iostream>
#include <cstdint>
#include "f_parser/f_packet.h"
#include "fifo_atomic.h"



[[maybe_unused]] const uint8_t test_rc_nav[]{
  0xAC,
  0x33,
  0x01, 0x01,
  0x02, 0x02,
  0x03, 0x03,
  0x04, 0x04,
  0xeb, 0x7b,
  0x12, 0x34,

  0xab,
  0x33,
  0x11, 0x01,
  0x22, 0x02,
  0x33, 0x03,
  0x44, 0x04,
  0x55, 0x05,
  0x66, 0x06,
  0xA5, 0x5A,
  0xd5, 0x41,
  0x12, 0x34
};


device_context context;
packet_parser parser(context);




//std::cout << "RC nav" << std::endl;
//for (auto i: context.rc_nav.analog_channels) {
//  std::cout << std::hex << i << std::endl;
//}
//std::cout << "RC full" << std::endl;
//
//for (auto i: context.rc_full.analog_channels) {
//  std::cout << std::hex <<  i << std::endl;
//}
//
//std::cout << "  digital " << std::hex << context.rc_full.digital_channels << std::endl;

TEST_CASE("parser class") {

  //fill fifo
  for (auto i: test_rc_nav) {
    parser.add_byte(i);
  }
  parser.parse_buffer();
  parser.parse_buffer();
  parser.parse_buffer();
  parser.parse_buffer();

  SUBCASE("parse correct nav packet") {
    int cnt = 0;
    for (auto i : context.rc_nav.analog_channels) {
      CHECK(i == ((uint16_t*)(&test_rc_nav[2]))[cnt++]);
    }
  }

  //TODO: add incorrect packets

  SUBCASE("parse correct full packet") {
    int cnt = 0;
    for (auto i : context.rc_full.analog_channels) {
      CHECK(i == ((uint16_t*)(&test_rc_nav[16]))[cnt++]);
    }
    CHECK(0x5aa5 == ((uint16_t*)(&test_rc_nav[28]))[0]);
  }

}
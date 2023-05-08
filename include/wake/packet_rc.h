#pragma once
#include <cstdint>



namespace wake {
/// ### Пакет управления короткий
///
///- Сигнатура 0xС0
///- Адрес 1Б
///- Команда 0x20
///- Длина 0x08
///- Каналы int16[4]
///- crc 1Б
///---
/// 13 Байт. В идеале ужать до 12
#pragma pack(push, 1)
struct packet_rc_small {
  constexpr static uint8_t command = 0x20;
  uint16_t channels[4];
};
#pragma pack(pop)

#pragma pack(push, 1)
struct packet_rc {
  constexpr static uint8_t command = 0x21;
  uint16_t channels[8];
};
#pragma pack(pop)

}


#ifndef PROTOCOL_HPP
#define PROTOCOL_HPP

#include <cstdint>

constexpr uint8_t PACKET_STX = 0x02;
constexpr uint8_t PACKET_ETX = 0x03;

constexpr uint8_t PACKET_TYP_CMD  = 0x90;
constexpr uint8_t PACKET_TYP_RESP = 0x91;
constexpr uint8_t PACKET_TYP_STRM = 0x92;

constexpr int PACKET_LEN_MAX = 255;

constexpr uint8_t PACKET_CMD_STRM_DAT  = 0xA0;
constexpr uint8_t PACKET_CMD_STRM      = 0xA1;
constexpr uint8_t PACKET_CMD_THROT     = 0xA2;
constexpr uint8_t PACKET_CMD_PWM       = 0xA3;
constexpr uint8_t PACKET_CMD_LED_RED   = 0xD0;
constexpr uint8_t PACKET_CMD_LED_GREEN = 0xD1;
constexpr uint8_t PACKET_CMD_LED_BLUE  = 0xD2;

class Packet
{
public:
    uint8_t typ;
    uint8_t len;
    uint8_t dat[PACKET_LEN_MAX];
};

#endif

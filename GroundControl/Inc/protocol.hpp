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

constexpr uint8_t PACKET_ACK = 0x06;
constexpr uint8_t PACKET_NACK = 0x15;

constexpr uint8_t PACKET_ERR_OK        = 0xE0;
constexpr uint8_t PACKET_ERR_ETX_MIS   = 0xE1;
constexpr uint8_t PACKET_ERR_CKS_MIS   = 0xE2;
constexpr uint8_t PACKET_ERR_TYP_INVAL = 0xE3;
constexpr uint8_t PACKET_ERR_CMD_INVAL = 0xE4;
constexpr uint8_t PACKET_ERR_LEN_INVAL = 0xE5;
constexpr uint8_t PACKET_ERR_ARG_INVAL = 0xE6;

constexpr uint8_t PACKET_DAT_ACC       = 0x10;
constexpr uint8_t PACKET_DAT_ANG       = 0x11;
constexpr uint8_t PACKET_DAT_MAG       = 0x12;
constexpr uint8_t PACKET_DAT_RAW_ACC   = 0x18;
constexpr uint8_t PACKET_DAT_RAW_ANG   = 0x19;
constexpr uint8_t PACKET_DAT_RAW_MAG   = 0x1A;
constexpr uint8_t PACKET_DAT_KF_QUAT   = 0x20;
constexpr uint8_t PACKET_DAT_KF_RPY    = 0x21;
constexpr uint8_t PACKET_DAT_MEAS_QUAT = 0x28;
constexpr uint8_t PACKET_DAT_MEAS_RPY  = 0x29;
constexpr uint8_t PACKET_DAT_COV_STATE = 0x30;
constexpr uint8_t PACKET_DAT_COV_PROC  = 0x31;
constexpr uint8_t PACKET_DAT_COV_MEAS  = 0x32;
constexpr uint8_t PACKET_DAT_RF_CH1    = 0x40;
constexpr uint8_t PACKET_DAT_RF_CH2    = 0x41;
constexpr uint8_t PACKET_DAT_RF_CH3    = 0x42;
constexpr uint8_t PACKET_DAT_RF_CH4    = 0x43;
constexpr uint8_t PACKET_DAT_RF_CH5    = 0x44;
constexpr uint8_t PACKET_DAT_RF_CH6    = 0x45;
constexpr uint8_t PACKET_DAT_RF_CH7    = 0x46;
constexpr uint8_t PACKET_DAT_RF_CH8    = 0x47;
constexpr uint8_t PACKET_DAT_RF_CH9    = 0x48;
constexpr uint8_t PACKET_DAT_RF_CH10   = 0x49;
constexpr uint8_t PACKET_DAT_RF_CH11   = 0x4A;
constexpr uint8_t PACKET_DAT_RF_CH12   = 0x4B;
constexpr uint8_t PACKET_DAT_RF_CH13   = 0x4C;
constexpr uint8_t PACKET_DAT_RF_CH14   = 0x4D;
constexpr uint8_t PACKET_DAT_RF_CH15   = 0x4E;
constexpr uint8_t PACKET_DAT_RF_CH16   = 0x4F;

class Packet
{
public:
    uint8_t typ;
    uint8_t len;
    uint8_t dat[PACKET_LEN_MAX];
};

#endif
